#include "Model.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Math/MathFunction.h"
#include <cassert>
#include <fstream>
#include <sstream>

void Model::CreateFromOBJ(const std::string& directoryPath, const std::string& filename, DrawPass drawPass)
{
	//モデルデータの読み込み
	modelData_ = LoadObjFile(directoryPath, filename);

	//描画パスを設定
	drawPass_ = drawPass;

	//テクスチャを読み込む
	if (modelData_.material.textureFilePath != "")
	{
		textureName_ = modelData_.material.textureFilePath;
		TextureManager::Load(modelData_.material.textureFilePath);
	}

	//頂点バッファの作成
	CreateVertexBuffer();

	//マテリアル用のリソースの作成
	CreateMaterialConstBuffer();
}

void Model::Update()
{
	//行列の作成
	Matrix4x4 uvTransformMatrix = Mathf::MakeScaleMatrix({ uvScale_.x,uvScale_.y,1.0f });
	uvTransformMatrix = uvTransformMatrix * Mathf::MakeRotateZMatrix(uvRotation_);
	uvTransformMatrix = uvTransformMatrix * Mathf::MakeTranslateMatrix({ uvTranslation_.x,uvTranslation_.y,0.0f });

	//マテリアルデータの更新
	ConstBuffDataMaterial* materialData = static_cast<ConstBuffDataMaterial*>(materialConstBuffer_->Map());
	materialData->color = color_;
	materialData->uvTransform = uvTransformMatrix;
	materialConstBuffer_->Unmap();
}

void Model::Draw(const WorldTransform& worldTransform, const Camera& camera)
{
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();
	TextureManager* textureManager = TextureManager::GetInstance();
	commandContext->SetVertexBuffer(vertexBufferView_);
	commandContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandContext->SetConstantBuffer(0, materialConstBuffer_->GetGpuVirtualAddress());
	commandContext->SetConstantBuffer(1, worldTransform.GetConstantBuffer()->GetGpuVirtualAddress());
	commandContext->SetConstantBuffer(2, camera.GetConstantBuffer()->GetGpuVirtualAddress());
	commandContext->SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, textureManager->GetDescriptorHeap()->GetDescriptorHeap());
	commandContext->SetDescriptorTable(3, textureManager->GetDescriptorHandle(textureName_));
	commandContext->DrawInstanced(UINT(modelData_.vertices.size()), 1);
}

Model::ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	ModelData modelData;//構築するModelData
	std::vector<Vector4> positions;//位置
	std::vector<Vector3> normals;//法線
	std::vector<Vector2> texcoords;//テクスチャ座標
	std::string line;//ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//とりあえず開けなかったら止める

	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む

		//identifierに応じた処理
		if (identifier == "v")
		{
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.z *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt")
		{
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn")
		{
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.z *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f")
		{
			VertexDataPosUVNormal triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex)
			{
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element)
				{
					std::string index;
					std::getline(v, index, '/');// /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position,texcoord,normal };
			}
			//頂点を逆順で登録することで、回り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib")
		{
			//materialTempalteLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}

Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	MaterialData materialData;//構築するMaterialData
	std::string line;//ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//とりあえず開けなかったら止める

	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd")
		{
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}

void Model::CreateVertexBuffer()
{
	//頂点バッファを作成
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sizeof(VertexDataPosUVNormal) * modelData_.vertices.size());

	//頂点バッファビューを作成
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexDataPosUVNormal) * modelData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUVNormal);

	//頂点バッファにデータを書き込む
	VertexDataPosUVNormal* vertexData = static_cast<VertexDataPosUVNormal*>(vertexBuffer_->Map());
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexDataPosUVNormal) * modelData_.vertices.size());
	vertexBuffer_->Unmap();
}

void Model::CreateMaterialConstBuffer()
{
	materialConstBuffer_ = std::make_unique<UploadBuffer>();
	materialConstBuffer_->Create(sizeof(ConstBuffDataMaterial));

	ConstBuffDataMaterial* materialData = static_cast<ConstBuffDataMaterial*>(materialConstBuffer_->Map());
	materialData->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData->uvTransform = Mathf::MakeIdentity4x4();
	materialConstBuffer_->Unmap();
}

//Model::ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& filename)
//{
//	ModelData modelData;
//	Assimp::Importer importer;
//	std::string filePath = directoryPath + "/" + filename;
//	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
//	assert(scene->HasMeshes());//メッシュがないのは対応しない
//
//	//Meshの解析
//	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
//	{
//		aiMesh* mesh = scene->mMeshes[meshIndex];
//		assert(mesh->HasNormals());//法線がないMeshは今回は非対応
//		assert(mesh->HasTextureCoords(0));//TexcoordがないMeshは今回は非対応
//		//ここからMeshの中身(Face)の解析を行っていく
//		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
//		{
//			aiFace& face = mesh->mFaces[faceIndex];
//			assert(face.mNumIndices == 3);//三角形のみサポート
//			//ここからFaceの中身(Vertex)の解析を行っていく
//			for (uint32_t element = 0; element < face.mNumIndices; ++element)
//			{
//				uint32_t vertexIndex = face.mIndices[element];
//				aiVector3D& position = mesh->mVertices[vertexIndex];
//				aiVector3D& normal = mesh->mNormals[vertexIndex];
//				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
//				VertexDataPosUVNormal vertex{};
//				vertex.position = { position.x,position.y,position.z,1.0f };
//				vertex.normal = { normal.x,normal.y,normal.z };
//				vertex.texcoord = { texcoord.x,texcoord.y };
//				//aiProcess_MakeLeftHandedはz*=-1で、右手->左手に変換するので手動で対処
//				vertex.position.x *= -1.0f;
//				vertex.normal.x *= -1.0f;
//				modelData.vertices.push_back(vertex);
//			}
//		}
//	}
//
//	//Materialの解析
//	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex)
//	{
//		aiMaterial* material = scene->mMaterials[materialIndex];
//		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
//		{
//			aiString textureFilePath;
//			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
//			modelData.material.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
//		}
//	}
//
//	//Nodeの解析
//	modelData.rootNode = ReadNode(scene->mRootNode);
//
//	return modelData;
//}
//
//Model::Node Model::ReadNode(aiNode* node) 
//{
//	Node result{};
//	aiMatrix4x4 aiLocalMatrix = node->mTransformation;//nodeのlocalMatrixを取得
//	aiLocalMatrix.Transpose();//列ベクトル形式を行ベクトル形式に転置
//	for (uint32_t i = 0; i < 4; i++)
//	{
//		for (uint32_t j = 0; j < 4; j++)
//		{
//			result.localMatrix.m[i][j] = aiLocalMatrix[i][j];
//		}
//	}
//	result.name = node->mName.C_Str();//Node名を格納
//	result.children.resize(node->mNumChildren);//子供の数だけ確保
//	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
//	{
//		//再帰的に読んで階層構造を作っていく
//		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
//	}
//	return result;
//}
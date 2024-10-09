#include "ModelManager.h"
#include "Engine/Math/MathFunction.h"

ModelManager* ModelManager::instance_ = nullptr;
const std::string ModelManager::kBaseDirectory = "Application/Resources/Models";

ModelManager* ModelManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new ModelManager();
	}
	return instance_;
}

void ModelManager::Destroy()
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

Model* ModelManager::CreateFromModelFile(const std::string& modelName, DrawPass drawPass)
{
	Model* model = ModelManager::GetInstance()->CreateInternal(modelName, drawPass);
	return model;
}

Model* ModelManager::CreateInternal(const std::string& modelName, DrawPass drawPass)
{
	//既存のモデルから再利用可能なものを探す
	Model* reusableModel = FindReusableModel(modelName);
	if (reusableModel)
	{
		return reusableModel;
	}

	//モデルデータが存在するか確認
	auto modelDataIt = modelDatas_.find(modelName);
	if (modelDataIt != modelDatas_.end())
	{
		return CreateModelFromData(modelDataIt->second, modelName, drawPass);
	}

	//モデルファイルを探索してロード
	std::string fileName = FindModelFile(modelName);

	//モデルデータを読み込み
	Model::ModelData modelData = LoadModelFile(kBaseDirectory + "/" + modelName, fileName);
	//モデルデータを保存
	modelDatas_[modelName] = modelData;

	//モデルを生成して返す
	return CreateModelFromData(modelData, modelName, drawPass);
}

Model* ModelManager::FindReusableModel(const std::string& modelName)
{
	auto it = models_.find(modelName);
	if (it != models_.end())
	{
		for (const auto& model : it->second)
		{
			//モデルが使用されていないかどうかを確認
			if (!model->GetIsInUse())
			{
				//使用されていないモデルを返す
				model->Acquire();
				return model.get();
			}
		}
	}

	//見つからなかった場合はnullptrを返す
	return nullptr;
}

Model* ModelManager::CreateModelFromData(const Model::ModelData& modelData, const std::string& modelName, DrawPass drawPass)
{
	Model* model = new Model();
	model->Initialize(modelData, drawPass);
	models_[modelName].emplace_back(std::unique_ptr<Model>(model));
	return model;
}

std::string ModelManager::FindModelFile(const std::string& modelName)
{
	std::string directoryPath = kBaseDirectory + "/" + modelName;
	if (std::filesystem::exists(directoryPath + "/" + modelName + ".gltf"))
	{
		return modelName + ".gltf";
	}
	else if (std::filesystem::exists(directoryPath + "/" + modelName + ".obj"))
	{
		return modelName + ".obj";
	}
	return "";
}

Model::ModelData ModelManager::LoadModelFile(const std::string& directoryPath, const std::string& filename)
{
	Model::ModelData modelData;
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());//メッシュがないのは対応しない

	//Meshの解析
	modelData.meshData.resize(scene->mNumMeshes);
	modelData.skinClusterData.resize(scene->mNumMeshes);
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
	{
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());//法線がないMeshは今回は非対応
		assert(mesh->HasTextureCoords(0));//TexcoordがないMeshは今回は非対応
		modelData.meshData[meshIndex].vertices.resize(mesh->mNumVertices);//最初に頂点数分のメモリを確保しておく
		modelData.meshData[meshIndex].materialIndex = mesh->mMaterialIndex;
		//頂点を解析
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
		{
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
			//右手系->左手系への変換を忘れずに
			modelData.meshData[meshIndex].vertices[vertexIndex].position = { -position.x,position.y,position.z,1.0f };
			modelData.meshData[meshIndex].vertices[vertexIndex].normal = { -normal.x,normal.y,normal.z };
			modelData.meshData[meshIndex].vertices[vertexIndex].texcoord = { texcoord.x,texcoord.y };
		}
		//Indexを解析する
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
		{
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);//三角形のみサポート

			for (uint32_t element = 0; element < face.mNumIndices; ++element)
			{
				uint32_t vertexIndex = face.mIndices[element];
				modelData.meshData[meshIndex].indices.push_back(vertexIndex);
			}
		}
		//SkinCluster構築用のデータを取得
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			//Jointごとに格納領域を作る
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			Model::JointWeightData& jointWeightData = modelData.skinClusterData[meshIndex][jointName];

			//InverseBindPoseMatrixの抽出
			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();//BindPoseMatrixに戻す
			aiVector3D scale, translate;
			aiQuaternion rotate;
			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);//成分を抽出
			//左手系のBindPoseMatrixを作る
			Matrix4x4 bindPoseMatrix = Mathf::MakeAffineMatrix({ scale.x,scale.y,scale.z },
				{ rotate.x,-rotate.y,-rotate.z,rotate.w },
				{ -translate.x,translate.y,translate.z });
			//InverseBindPoseMatrixにする
			jointWeightData.inverseBindPoseMatrix = Mathf::Inverse(bindPoseMatrix);

			//Weight情報を取り出す
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
			{
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight,bone->mWeights[weightIndex].mVertexId });
			}
		}
	}

	//Materialの解析
	modelData.materialData.resize(scene->mNumMaterials);
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex)
	{
		aiMaterial* material = scene->mMaterials[materialIndex];
		aiColor4D color;
		aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color);
		modelData.materialData[materialIndex].color = Vector4(color.r, color.g, color.b, color.a);
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0)
		{
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.materialData[materialIndex].textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}
	}

	//Nodeの解析
	modelData.rootNode = ReadNode(scene->mRootNode);

	return modelData;
}

Model::Node ModelManager::ReadNode(aiNode* node)
{
	Model::Node result{};
	aiVector3D scale, translate;
	aiQuaternion rotate;
	node->mTransformation.Decompose(scale, rotate, translate);//assimpの行列からSRTを抽出する関数を利用
	result.scale = { scale.x,scale.y,scale.z };//Scaleはそのまま
	result.rotate = { rotate.x,-rotate.y,-rotate.z,rotate.w };//X軸を反転、さらに回転方向が逆なので軸を反転させる
	result.translate = { -translate.x,translate.y,translate.z };//X軸を反転
	result.localMatrix = Mathf::MakeAffineMatrix(result.scale, result.rotate, result.translate);
	result.name = node->mName.C_Str();//Node名を格納
	result.children.resize(node->mNumChildren);//子供の数だけ確保
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
	{
		//再帰的に読んで階層構造を作っていく
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}
	return result;
}
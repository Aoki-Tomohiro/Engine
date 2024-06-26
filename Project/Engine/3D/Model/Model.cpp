#include "Model.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/3D/Primitive/LineRenderer.h"

void Model::Initialize(const ModelData& modelData, const std::vector<Animation::AnimationData>& animationData, const DrawPass drawPass)
{
	//モデルデータの初期化
	modelData_ = modelData;

	//アニメーションの作成
	animation_ = std::make_unique<Animation>();
	animation_->Initialize(animationData, modelData_.rootNode);

	//メッシュの作成
	for (uint32_t i = 0; i < modelData_.meshData.size(); ++i)
	{
		Mesh* mesh = new Mesh();
		mesh->Initialize(modelData_.meshData[i], animation_->GetSkeleton(), modelData_.skinClusterData[i]);
		meshes_.push_back(std::unique_ptr<Mesh>(mesh));
		if (mesh->GetHasSkinCluster())
		{
			hasSkinCluster_ = true;
		}
	}

	//マテリアルの作成
	for (uint32_t i = 0; i < modelData_.materialData.size(); ++i)
	{
		Material* material = new Material();
		material->Initialize(modelData_.materialData[i].textureFilePath);
		materials_.push_back(std::unique_ptr<Material>(material));
	}

	//Boneの作成
	if (hasSkinCluster_)
	{
		Animation::Skeleton skeleton = animation_->GetSkeleton();
		CreateBoneVertices(skeleton, skeleton.root, boneVertices_);
		CreateBoneVertexBuffer();
	}

	//描画パスを設定
	drawPass_ = drawPass;
}

void Model::Update(WorldTransform& worldTransform, const std::string& animationName)
{
	//アニメーションの更新を行って、骨ごとのLocal情報を更新する
	animation_->ApplyAnimation(worldTransform, modelData_.rootNode.name, animationName);

	//アニメーションの更新
	animation_->Update();

	//メッシュの更新
	for (uint32_t i = 0; i < modelData_.meshData.size(); ++i)
	{
		meshes_[i]->Update(animation_->GetSkeleton().joints);
	}

	//マテリアルの更新
	for (std::unique_ptr<Material>& material : materials_)
	{
		material->Update();
	}

	//RootのMatrixを適用
	if (!hasSkinCluster_)
	{
		worldTransform.matWorld_ = modelData_.rootNode.localMatrix * worldTransform.matWorld_;
		worldTransform.TransferMatrix();
	}
}

void Model::Draw(const WorldTransform& worldTransform, const Camera& camera)
{
	//レンダラーのインスタンスを取得
	Renderer* renderer_ = Renderer::GetInstance();

	//SortObjectの追加
	for (uint32_t i = 0; i < meshes_.size(); ++i)
	{
		//マテリアルのインデックスを取得
		uint32_t materialIndex = meshes_[i]->GetMaterialIndex();

		//Objectの追加
		renderer_->AddObject(meshes_[i]->GetVertexBufferView(), meshes_[i]->GetIndexBufferView(), materials_[materialIndex]->GetConstantBuffer()->GetGpuVirtualAddress(),
			worldTransform.GetConstantBuffer()->GetGpuVirtualAddress(), camera.GetConstantBuffer()->GetGpuVirtualAddress(),
			materials_[materialIndex]->GetTexture()->GetSRVHandle(), materials_[materialIndex]->GetMaskTexture()->GetSRVHandle(), UINT(meshes_[i]->GetIndicesSize()), drawPass_);

		//SkinClusterを持っている場合
		if (meshes_[i]->GetHasSkinCluster())
		{
			//SkinningObjectの追加
			renderer_->AddSkinningObject(meshes_[i]->GetPaletteResource()->GetSRVHandle(), meshes_[i]->GetInputVerticesBuffer()->GetSRVHandle(),
				meshes_[i]->GetInfluenceResource()->GetSRVHandle(), meshes_[i]->GetSkinningInformationBuffer()->GetGpuVirtualAddress(),
				meshes_[i]->GetOutputVerticesBuffer(), UINT(meshes_[i]->GetVerticesSize()));
		}
	}

	//SkinClusterを持っている場合
	if (hasSkinCluster_)
	{
		//Boneの描画
		if (isBoneVisible_)
		{
			//頂点データの更新
			UpdateVertexData(animation_->GetSkeleton(), animation_->GetSkeleton().root, boneVertices_);
			
			//Boneの追加
			renderer_->AddBone(boneVertexBufferView_, worldTransform.GetConstantBuffer()->GetGpuVirtualAddress(), camera.GetConstantBuffer()->GetGpuVirtualAddress(), UINT(boneVertices_.size()));
		}
	}
}

void Model::CreateBoneVertexBuffer()
{
	//頂点バッファを作成
	boneVertexBuffer_ = std::make_unique<UploadBuffer>();
	boneVertexBuffer_->Create(sizeof(Vector4) * boneVertices_.size());

	//頂点バッファビューを作成
	boneVertexBufferView_.BufferLocation = boneVertexBuffer_->GetGpuVirtualAddress();
	boneVertexBufferView_.SizeInBytes = UINT(sizeof(Vector4) * boneVertices_.size());
	boneVertexBufferView_.StrideInBytes = sizeof(Vector4);

	//頂点バッファにデータを書き込む
	Vector4* vertexData = static_cast<Vector4*>(boneVertexBuffer_->Map());
	std::memcpy(vertexData, boneVertices_.data(), sizeof(Vector4) * boneVertices_.size());
	boneVertexBuffer_->Unmap();
}

void Model::CreateBoneVertices(const Animation::Skeleton& skeleton, int32_t parentIndex, std::vector<Vector4>& vertices)
{
	const Animation::Joint& parentJoint = skeleton.joints[parentIndex];
	for (int32_t childIndex : parentJoint.children)
	{
		const Animation::Joint& childJoint = skeleton.joints[childIndex];
		vertices.push_back({ parentJoint.skeletonSpaceMatrix.m[3][0],parentJoint.skeletonSpaceMatrix.m[3][1],parentJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		vertices.push_back({ childJoint.skeletonSpaceMatrix.m[3][0],childJoint.skeletonSpaceMatrix.m[3][1],childJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		CreateBoneVertices(skeleton, childIndex, vertices);
	}
}

void Model::UpdateVertexData(const Animation::Skeleton& skeleton, int32_t parentIndex, std::vector<Vector4>& vertices)
{
	//頂点データをクリア
	vertices.clear();

	//頂点データの作成
	CreateBoneVertices(skeleton, skeleton.root, boneVertices_);

	//頂点データの書き込み
	Vector4* vertexData = static_cast<Vector4*>(boneVertexBuffer_->Map());
	std::memcpy(vertexData, vertices.data(), sizeof(Vector4) * vertices.size());
	boneVertexBuffer_->Unmap();
}
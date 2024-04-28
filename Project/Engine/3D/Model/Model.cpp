#include "Model.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Math/MathFunction.h"

void Model::Create(const ModelData& modelData, const Animation::AnimationData& animationData, DrawPass drawPass)
{
	//モデルデータを設定
	modelData_ = modelData;

	//メッシュの作成
	mesh_ = std::make_unique<Mesh>();
	mesh_->Initialize(modelData_.vertices, modelData_.indices);

	//マテリアルの作成
	material_ = std::make_unique<Material>();
	material_->Initialize(modelData_.material.textureFilePath);

	//アニメーションの作成
	animation_ = std::make_unique<Animation>();
	animation_->Initialize(animationData, modelData_.rootNode);

	//描画パスを設定
	drawPass_ = drawPass;
}

void Model::Update(WorldTransform& worldTransform)
{
	//アニメーションの更新
	animation_->Update(modelData_.rootNode.name);

	//アニメーションを適用
	if (animation_->IsPlaying())
	{
		worldTransform.matWorld_ = animation_->GetLocalMatrix() * worldTransform.matWorld_;
	}
}

void Model::Draw(WorldTransform& worldTransform, const Camera& camera)
{
	//マテリアルの更新
	material_->Update();

	//RootのMatrixを適用
	worldTransform.matWorld_ = modelData_.rootNode.localMatrix * worldTransform.matWorld_;
	worldTransform.TransferMatrix();

	//レンダラーのインスタンスを取得
	Renderer* renderer_ = Renderer::GetInstance();
	//SortObjectの追加
	renderer_->AddObject(mesh_->GetVertexBufferView(),mesh_->GetIndexBufferView(), material_->GetConstantBuffer()->GetGpuVirtualAddress(),
		worldTransform.GetConstantBuffer()->GetGpuVirtualAddress(), camera.GetConstantBuffer()->GetGpuVirtualAddress(),
		material_->GetTexture()->GetSRVHandle(), UINT(mesh_->GetIndicesSize()), drawPass_);
}
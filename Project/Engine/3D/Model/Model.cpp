#include "Model.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Math/MathFunction.h"

void Model::Create(const ModelData& modelData, DrawPass drawPass)
{
	//モデルデータを設定
	modelData_ = modelData;

	//メッシュの作成
	mesh_ = std::make_unique<Mesh>();
	mesh_->Initialize(modelData_.vertices);

	//マテリアルの作成
	material_ = std::make_unique<Material>();
	material_->Initialize();

	//描画パスを設定
	drawPass_ = drawPass;
}

void Model::Update()
{
	material_->Update();
}

void Model::Draw(const WorldTransform& worldTransform, const Camera& camera)
{
	//RootのMatrixを適用
	worldTransform.matWorld_ *= modelData_.rootNode.localMatrix;
	worldTransform.TransferMatrix();

	//レンダラーのインスタンスを取得
	Renderer* renderer_ = Renderer::GetInstance();
	//SortObjectの追加
	renderer_->AddObject(mesh_->GetVertexBufferView(), material_->GetConstantBuffer()->GetGpuVirtualAddress(),
		worldTransform.GetConstantBuffer()->GetGpuVirtualAddress(), camera.GetConstantBuffer()->GetGpuVirtualAddress(),
		material_->GetTexture()->GetSRVHandle(), UINT(mesh_->GetVerticesSize()), drawPass_);
}
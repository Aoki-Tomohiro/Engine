#include "Model.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Math/MathFunction.h"

void Model::Create(const ModelData& modelData, DrawPass drawPass)
{
	//モデルデータを設定
	modelData_ = modelData;

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

void Model::UpdateMaterailConstBuffer()
{
	//行列の作成
	Matrix4x4 uvTransformMatrix = Mathf::MakeScaleMatrix({ uvScale_.x,uvScale_.y,1.0f });
	uvTransformMatrix = uvTransformMatrix * Mathf::MakeRotateZMatrix(uvRotation_);
	uvTransformMatrix = uvTransformMatrix * Mathf::MakeTranslateMatrix({ uvTranslation_.x,uvTranslation_.y,0.0f });

	//マテリアルデータの更新
	ConstBuffDataMaterial* materialData = static_cast<ConstBuffDataMaterial*>(materialConstBuffer_->Map());
	materialData->color = color_;
	materialData->uvTransform = uvTransformMatrix;
	materialData->enableLighting = enableLighting_;
	materialData->diffuseReflectionType = diffuseReflectionType_;
	materialData->specularReflectionType = specularReflectionType_;
	materialData->shininess = shininess_;
	materialData->specularColor = specularColor_;
	materialConstBuffer_->Unmap();
}

void Model::Draw(const WorldTransform& worldTransform, const Camera& camera)
{
	//マテリアルの更新
	UpdateMaterailConstBuffer();

	//レンダラーのインスタンスを取得
	Renderer* renderer_ = Renderer::GetInstance();
	//SortObjectの追加
	renderer_->AddObject(vertexBufferView_, materialConstBuffer_->GetGpuVirtualAddress(),
		worldTransform.GetConstantBuffer()->GetGpuVirtualAddress(), camera.GetConstantBuffer()->GetGpuVirtualAddress(),
		TextureManager::GetInstance()->GetDescriptorHandle(textureName_), UINT(modelData_.vertices.size()), drawPass_);

	//ID3D12GraphicsCommandList* commandList = GraphicsCore::GetInstance()->GetCommandList();
	//TextureManager* textureManager = TextureManager::GetInstance();
	//commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//commandList->SetGraphicsRootConstantBufferView(0, materialConstBuffer_->GetGpuVirtualAddress());
	//commandList->SetGraphicsRootConstantBufferView(1, worldTransform.GetConstantBuffer()->GetGpuVirtualAddress());
	//commandList->SetGraphicsRootConstantBufferView(2, camera.GetConstantBuffer()->GetGpuVirtualAddress());
	//commandList->SetGraphicsRootDescriptorTable(3, textureManager->GetDescriptorHandle(textureName_));
	//commandList->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
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
	//マテリアル用のリソースの作成
	materialConstBuffer_ = std::make_unique<UploadBuffer>();
	materialConstBuffer_->Create(sizeof(ConstBuffDataMaterial));

	//マテリアル用のリソースに書き込む
	UpdateMaterailConstBuffer();
}
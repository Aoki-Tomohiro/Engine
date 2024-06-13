#include "Skybox.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Math/MathFunction.h"

Skybox* Skybox::Create(const std::string& textureName)
{
	Skybox* skybox = new Skybox();
	skybox->Initialize(textureName);
	return skybox;
}

void Skybox::Draw(const WorldTransform& worldTransform, const Camera& camera)
{
	GraphicsContext* graphicsContext = GraphicsCore::GetInstance()->GetGraphicsContext();
	TextureManager* textureManager = TextureManager::GetInstance();
	graphicsContext->SetVertexBuffer(vertexBufferView_);
	graphicsContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphicsContext->SetIndexBuffer(indexBufferView_);
	graphicsContext->SetConstantBuffer(0, materialConstBuffer_->GetGpuVirtualAddress());
	graphicsContext->SetConstantBuffer(1, worldTransform.GetConstantBuffer()->GetGpuVirtualAddress());
	graphicsContext->SetConstantBuffer(2, camera.GetConstantBuffer()->GetGpuVirtualAddress());
	graphicsContext->SetDescriptorTable(3, texture_->GetSRVHandle());
	graphicsContext->DrawIndexedInstanced(kMaxIndices, 1);
}

void Skybox::Initialize(const std::string& textureName)
{
	//テクスチャを設定
	SetTexture(textureName);
	
	//頂点バッファの作成
	CreateVertexBuffer();

	//IndexBufferの作成
	CreateIndexBuffer();

	//マテリアル用のリソースの作成
	CreateMaterialResource();
}

void Skybox::CreateVertexBuffer()
{
	//右面。描画インデックスは[0,1,2][2,1,3]で内側を向く
	vertices_[0].position = { 1.0f,1.0f,1.0f,1.0f };
	vertices_[1].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertices_[2].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertices_[3].position = { 1.0f,-1.0f,-1.0f,1.0f };
	//左面。描画インデックスは[4,5,6][6,5,7]
	vertices_[4].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertices_[5].position = { -1.0f,1.0f,1.0f,1.0f };
	vertices_[6].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertices_[7].position = { -1.0f,-1.0f,1.0f,1.0f };
	//前面。描画インデックスは[8,9,10][10,9,11]
	vertices_[8].position = { -1.0f,1.0f,1.0f,1.0f };
	vertices_[9].position = { 1.0f,1.0f,1.0f,1.0f };
	vertices_[10].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertices_[11].position = { 1.0f,-1.0f,1.0f,1.0f };
	//後面。描画インデックスは[12,13,14][14,13,15]
	vertices_[12].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertices_[13].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertices_[14].position = { 1.0f,-1.0f,-1.0f,1.0f };
	vertices_[15].position = { -1.0f,-1.0f,-1.0f,1.0f };
	//上面。描画インデックスは[16,17,18][18,17,19]
	vertices_[16].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertices_[17].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertices_[18].position = { -1.0f,1.0f,1.0f,1.0f };
	vertices_[19].position = { 1.0f,1.0f,1.0f,1.0f };
	//下面。描画インデックスは[20,21,22][22,21,23]
	vertices_[20].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertices_[21].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertices_[22].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertices_[23].position = { 1.0f,-1.0f,-1.0f,1.0f };

	//頂点リソースを作る
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sizeof(VertexDataPosUV) * kMaxVertices);
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexDataPosUV) * kMaxVertices;
	//頂点1つあたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUV);

	//頂点バッファにデータを書き込む
	VertexDataPosUV* vertexData = static_cast<VertexDataPosUV*>(vertexBuffer_->Map());
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexDataPosUV) * vertices_.size());
	vertexBuffer_->Unmap();
}

void Skybox::CreateIndexBuffer()
{
	//右面。描画インデックスは[0,1,2][2,1,3]で内側を向く
	indices_[0] = 0;
	indices_[1] = 1;
	indices_[2] = 2;
	indices_[3] = 2;
	indices_[4] = 1;
	indices_[5] = 3;
	//左面。描画インデックスは[4,5,6][6,5,7]
	indices_[6] = 4;
	indices_[7] = 5;
	indices_[8] = 6;
	indices_[9] = 6;
	indices_[10] = 5;
	indices_[11] = 7;
	//前面。描画インデックスは[8,9,10][10,9,11]
	indices_[12] = 8;
	indices_[13] = 9;
	indices_[14] = 10;
	indices_[15] = 10;
	indices_[16] = 9;
	indices_[17] = 11;
	//後面。描画インデックスは[12,13,14][14,13,15]
	indices_[18] = 12;
	indices_[19] = 13;
	indices_[20] = 14;
	indices_[21] = 14;
	indices_[22] = 13;
	indices_[23] = 15;
	//上面。描画インデックスは[16,17,18][18,17,19]
	indices_[24] = 16;
	indices_[25] = 17;
	indices_[26] = 18;
	indices_[27] = 18;
	indices_[28] = 17;
	indices_[29] = 19;
	//下面。描画インデックスは[20,21,22][22,21,23]
	indices_[30] = 20;
	indices_[31] = 21;
	indices_[32] = 22;
	indices_[33] = 22;
	indices_[34] = 21;
	indices_[35] = 23;

	//インデックスバッファの作成
	indexBuffer_ = std::make_unique<UploadBuffer>();
	indexBuffer_->Create(sizeof(uint32_t) * indices_.size());

	//インデックスバッファビューの作成
	indexBufferView_.BufferLocation = indexBuffer_->GetGpuVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * indices_.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスバッファのデータを書き込む
	uint32_t* indexData = static_cast<uint32_t*>(indexBuffer_->Map());
	std::memcpy(indexData, indices_.data(), sizeof(uint32_t) * indices_.size());
	indexBuffer_->Unmap();
}

void Skybox::CreateMaterialResource()
{
	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialConstBuffer_ = std::make_unique<UploadBuffer>();
	materialConstBuffer_->Create(sizeof(ConstBuffDataMaterial));

	//書き込むためのアドレスを取得
	ConstBuffDataMaterial* materialData = static_cast<ConstBuffDataMaterial*>(materialConstBuffer_->Map());
	//データを書き込む
	materialData->color = { 1.0f,1.0f,1.0f,1.0f };
	materialData->uvTransform = Mathf::MakeIdentity4x4();
	materialConstBuffer_->Unmap();
}

void Skybox::SetTexture(const std::string& textureName)
{
	//テクスチャを設定
	texture_ = TextureManager::GetInstance()->FindTexture(textureName);
	//テクスチャがなかったら止める
	assert(texture_);
}
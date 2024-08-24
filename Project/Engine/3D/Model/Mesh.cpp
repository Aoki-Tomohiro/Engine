#include "Mesh.h"

void Mesh::Initialize(const MeshData& meshData, const bool hasSkinCluster)
{
	//メッシュデータの初期化
	meshData_ = meshData;

	//頂点バッファの作成
	CreateVertexBuffer(hasSkinCluster);

	//インデックスバッファの作成
	CreateIndexBuffer();
}

void Mesh::CreateVertexBuffer(const bool hasSkinCluster)
{
	//InputVerticeBufferの作成
	inputVerticesBuffer_ = std::make_unique<StructuredBuffer>();
	inputVerticesBuffer_->Create((uint32_t)meshData_.vertices.size(), sizeof(VertexDataPosUVNormal));
	VertexDataPosUVNormal* inputVertexData = static_cast<VertexDataPosUVNormal*>(inputVerticesBuffer_->Map());
	std::memcpy(inputVertexData, meshData_.vertices.data(), sizeof(VertexDataPosUVNormal) * meshData_.vertices.size());
	inputVerticesBuffer_->Unmap();

	//OutputVerticesBufferの作成
	outputVerticesBuffer_ = std::make_unique<RWStructuredBuffer>();
	outputVerticesBuffer_->Create((uint32_t)meshData_.vertices.size(), sizeof(VertexDataPosUVNormal));

	//頂点バッファビューを作成
	vertexBufferView_.BufferLocation = hasSkinCluster ? outputVerticesBuffer_->GetGpuVirtualAddress() : inputVerticesBuffer_->GetGpuVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexDataPosUVNormal) * meshData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUVNormal);

	//SkinningInformationBufferの作成
	skinningInformationBuffer_ = std::make_unique<UploadBuffer>();
	skinningInformationBuffer_->Create(sizeof(uint32_t));
	uint32_t* skinningInformationData = static_cast<uint32_t*>(skinningInformationBuffer_->Map());
	*skinningInformationData = (uint32_t)meshData_.vertices.size();
	skinningInformationBuffer_->Unmap();
}

void Mesh::CreateIndexBuffer()
{
	//インデックスバッファの作成
	indexBuffer_ = std::make_unique<UploadBuffer>();
	indexBuffer_->Create(sizeof(uint32_t) * meshData_.indices.size());

	//インデックスバッファビューの作成
	indexBufferView_.BufferLocation = indexBuffer_->GetGpuVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * meshData_.indices.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスバッファのデータを書き込む
	uint32_t* indexData = static_cast<uint32_t*>(indexBuffer_->Map());
	std::memcpy(indexData, meshData_.indices.data(), sizeof(uint32_t) * meshData_.indices.size());
	indexBuffer_->Unmap();
}
#include "Mesh.h"

void Mesh::Initialize(const std::vector<VertexDataPosUVNormal>& vertices)
{
	//頂点データの初期化
	vertices_ = vertices;

	//頂点バッファを作成
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sizeof(VertexDataPosUVNormal) * vertices_.size());

	//頂点バッファビューを作成
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexDataPosUVNormal) * vertices_.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUVNormal);

	//更新
	Update();
}

void Mesh::Update()
{
	//頂点バッファにデータを書き込む
	VertexDataPosUVNormal* vertexData = static_cast<VertexDataPosUVNormal*>(vertexBuffer_->Map());
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexDataPosUVNormal) * vertices_.size());
	vertexBuffer_->Unmap();
}
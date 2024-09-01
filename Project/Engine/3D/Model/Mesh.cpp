#include "Mesh.h"

void Mesh::Initialize(const MeshData& meshData, const bool hasSkinCluster)
{
    // メッシュデータの初期化
    meshData_ = meshData;

    // 頂点バッファの作成
    CreateVertexBuffer(hasSkinCluster);

    // インデックスバッファの作成
    CreateIndexBuffer();
}

void Mesh::CreateVertexBuffer(const bool hasSkinCluster)
{
    if (hasSkinCluster)
    {
        //スキンクラスターを持っている場合の処理
        CreateInputVerticesBuffer();
        CreateOutputVerticesBuffer();
        CreateVertexBufferViewForSkinCluster();
        CreateSkinningInformationBuffer();
    }
    else
    {
        //スキンクラスターを持っていない場合の処理
        CreateVertexBufferWithoutSkinCluster();
    }
}

void Mesh::CreateInputVerticesBuffer()
{
    //InputVerticesBufferの作成
    inputVerticesBuffer_ = std::make_unique<StructuredBuffer>();
    inputVerticesBuffer_->Create(static_cast<uint32_t>(meshData_.vertices.size()), sizeof(VertexDataPosUVNormal));

    //頂点データのマッピングとコピー
    VertexDataPosUVNormal* inputVertexData = static_cast<VertexDataPosUVNormal*>(inputVerticesBuffer_->Map());
    std::memcpy(inputVertexData, meshData_.vertices.data(), sizeof(VertexDataPosUVNormal) * meshData_.vertices.size());
    inputVerticesBuffer_->Unmap();
}

void Mesh::CreateOutputVerticesBuffer()
{
    //OutputVerticesBufferの作成
    outputVerticesBuffer_ = std::make_unique<RWStructuredBuffer>();
    outputVerticesBuffer_->Create(static_cast<uint32_t>(meshData_.vertices.size()), sizeof(VertexDataPosUVNormal));
}

void Mesh::CreateVertexBufferViewForSkinCluster()
{
    //頂点バッファビューを作成
    vertexBufferView_.BufferLocation = outputVerticesBuffer_->GetGpuVirtualAddress();
    vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(VertexDataPosUVNormal) * meshData_.vertices.size());
    vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUVNormal);
}

void Mesh::CreateSkinningInformationBuffer()
{
    //SkinningInformationBufferの作成
    skinningInformationBuffer_ = std::make_unique<UploadBuffer>();
    skinningInformationBuffer_->Create(sizeof(uint32_t));

    //スキンニング情報を書き込む
    uint32_t* skinningInformationData = static_cast<uint32_t*>(skinningInformationBuffer_->Map());
    *skinningInformationData = static_cast<uint32_t>(meshData_.vertices.size());
    skinningInformationBuffer_->Unmap();
}

void Mesh::CreateVertexBufferWithoutSkinCluster()
{
    //スキンクラスターを持っていない場合の頂点バッファを作成
    vertexBuffer_ = std::make_unique<UploadBuffer>();
    vertexBuffer_->Create(sizeof(VertexDataPosUVNormal) * meshData_.vertices.size());

    //頂点バッファビューを作成
    vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
    vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(VertexDataPosUVNormal) * meshData_.vertices.size());
    vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUVNormal);

    //頂点バッファにデータを書き込む
    VertexDataPosUVNormal* vertexData = static_cast<VertexDataPosUVNormal*>(vertexBuffer_->Map());
    std::memcpy(vertexData, meshData_.vertices.data(), sizeof(VertexDataPosUVNormal) * meshData_.vertices.size());
    vertexBuffer_->Unmap();
}

void Mesh::CreateIndexBuffer()
{
    //インデックスバッファの作成
    indexBuffer_ = std::make_unique<UploadBuffer>();
    indexBuffer_->Create(sizeof(uint32_t) * meshData_.indices.size());

    //インデックスバッファビューの作成
    indexBufferView_.BufferLocation = indexBuffer_->GetGpuVirtualAddress();
    indexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * meshData_.indices.size());
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

    //インデックスバッファにデータを書き込む
    uint32_t* indexData = static_cast<uint32_t*>(indexBuffer_->Map());
    std::memcpy(indexData, meshData_.indices.data(), sizeof(uint32_t) * meshData_.indices.size());
    indexBuffer_->Unmap();
}
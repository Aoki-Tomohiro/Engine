#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/StructuredBuffer.h"
#include "Engine/Base/RWStructuredBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <vector>
#include <memory>
#include <span>

class Mesh
{
public:
	//メッシュデータ構造体
	struct MeshData
	{
		std::vector<VertexDataPosUVNormal> vertices;
		std::vector<uint32_t> indices;
		uint32_t materialIndex;
	};

	void Initialize(const MeshData& meshData, const bool hasSkinCluster);

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView_; };

	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return indexBufferView_; };

	const uint32_t GetMaterialIndex() const { return meshData_.materialIndex; };

	const size_t GetVerticesSize() const { return meshData_.vertices.size(); };

	const size_t GetIndicesSize() const { return meshData_.indices.size(); };

	StructuredBuffer* GetInputVerticesBuffer() const { return inputVerticesBuffer_.get(); };

	RWStructuredBuffer* GetOutputVerticesBuffer() const { return outputVerticesBuffer_.get(); };

	UploadBuffer* GetSkinningInformationBuffer() const { return skinningInformationBuffer_.get(); };

private:
	void CreateVertexBuffer(const bool hasSkinCluster);

	void CreateInputVerticesBuffer();

	void CreateOutputVerticesBuffer();

	void CreateVertexBufferViewForSkinCluster();

	void CreateSkinningInformationBuffer();

	void CreateVertexBufferWithoutSkinCluster();

	void CreateIndexBuffer();

private:
	MeshData meshData_{};

	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	std::unique_ptr<StructuredBuffer> inputVerticesBuffer_ = nullptr;

	std::unique_ptr<RWStructuredBuffer> outputVerticesBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::unique_ptr<UploadBuffer> indexBuffer_ = nullptr;

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	std::unique_ptr<UploadBuffer> skinningInformationBuffer_ = nullptr;
};


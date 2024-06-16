#pragma once
#include "Animation.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/StructuredBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <array>
#include <memory>
#include <span>
#include <vector>

class Mesh
{
public:
	struct MeshData
	{
		std::vector<VertexDataPosUVNormal> vertices;
		std::vector<uint32_t> indices;
		uint32_t materialIndex;
	};

	struct VertexWeightData
	{
		float weight;
		uint32_t vertexIndex;
	};

	struct JointWeightData
	{
		Matrix4x4 inverseBindPoseMatrix;
		std::vector<VertexWeightData> vertexWeights;
	};

	static const uint32_t kNumMaxInfluence = 4;
	struct VertexInfluence
	{
		std::array<float, kNumMaxInfluence> weights;
		std::array<int32_t, kNumMaxInfluence> jointIndices;
	};

	struct WellForGPU
	{
		Matrix4x4 skeletonSpaceMatrix;//位置用
		Matrix4x4 skeletonSpaceInverseTransposeMatrix;//法線用
	};

	struct SkinCluster
	{
		std::vector<Matrix4x4> inverseBindPoseMatrices;
		//Influence
		std::unique_ptr<UploadBuffer> influenceResource;
		D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
		std::span<VertexInfluence> mappedInfluence;
		//MatrixPalette
		std::unique_ptr<StructuredBuffer> paletteResource;
		std::span<WellForGPU> mappedPalette;
	};

	void Initialize(const MeshData& meshData, const Animation::Skeleton& skeleton, const std::map<std::string, JointWeightData>& skinClusterData);

	void Update(const std::vector<Animation::Joint>& joints);

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView_; };

	const D3D12_VERTEX_BUFFER_VIEW& GetInfluenceBufferView() const { return skinClusterData_.influenceBufferView; };

	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return indexBufferView_; };

	const size_t GetVerticesSize() const { return meshData_.vertices.size(); };

	const size_t GetIndicesSize() const { return meshData_.indices.size(); };

	const uint32_t GetMaterialIndex() const { return meshData_.materialIndex; };

	StructuredBuffer* GetPaletteResource() const { return skinClusterData_.paletteResource.get(); };

	const bool GetHasSkinCluster() const { return hasSkinCluster_; };

private:
	void CreateVertexBuffer();

	void CreateIndexBuffer();

	void CreateSkinCluster(const Animation::Skeleton& skeleton, const std::map<std::string, JointWeightData>& skinClusterData);

private:
	MeshData meshData_{};

	SkinCluster skinClusterData_{};

	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::unique_ptr<UploadBuffer> indexBuffer_ = nullptr;

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	bool hasSkinCluster_ = false;
};


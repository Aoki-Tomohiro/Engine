/**
 * @file Mesh.h
 * @brief メッシュを管理するファイル
 * @author 青木智滉
 * @date
 */

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

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="meshData">メッシュデータ</param>
	/// <param name="hasSkinCluster">スキンクラスターを持っているか</param>
	void Initialize(const MeshData& meshData, const bool hasSkinCluster);

	//頂点バッファビューを作成
	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView_; };

	//インデックスバッファビューを作成
	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return indexBufferView_; };

	//マテリアルのインデックスの取得
	const uint32_t GetMaterialIndex() const { return meshData_.materialIndex; };

	//頂点のサイズを取得
	const size_t GetVerticesSize() const { return meshData_.vertices.size(); };

	//インデックスのサイズを取得
	const size_t GetIndicesSize() const { return meshData_.indices.size(); };

	//入力用の頂点バッファを取得
	StructuredBuffer* GetInputVerticesBuffer() const { return inputVerticesBuffer_.get(); };

	//出力用の頂点バッファを取得
	RWStructuredBuffer* GetOutputVerticesBuffer() const { return outputVerticesBuffer_.get(); };

	//スキニング用のインフォメーションバッファを取得
	UploadBuffer* GetSkinningInformationBuffer() const { return skinningInformationBuffer_.get(); };

private:
	/// <summary>
	/// 頂点バッファを作成
	/// </summary>
	/// <param name="hasSkinCluster">スキンクラスターを持っているか</param>
	void CreateVertexBuffer(const bool hasSkinCluster);

	/// <summary>
	/// 入力用の頂点バッファを作成
	/// </summary>
	void CreateInputVerticesBuffer();

	/// <summary>
	/// 出力用の頂点バッファを作成
	/// </summary>
	void CreateOutputVerticesBuffer();

	/// <summary>
	/// スキンクラスターを基に頂点バッファビューを作成
	/// </summary>
	void CreateVertexBufferViewForSkinCluster();

	/// <summary>
	/// スキニングインフォメーションバッファを作成
	/// </summary>
	void CreateSkinningInformationBuffer();

	/// <summary>
	/// スキンクラスターを持っていない場合の頂点バッファを作成
	/// </summary>
	void CreateVertexBufferWithoutSkinCluster();

	/// <summary>
	/// インデックスバッファを作成
	/// </summary>
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


/**
 * @file Skybox.h
 * @brief スカイボックスを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/Texture.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include <array>

class Skybox
{
public:
	//頂点の最大数
	static const uint32_t kMaxVertices = 24;

	//インデックスの最大数
	static const uint32_t kMaxIndices = 36;

	/// <summary>
	/// スカイボックスの作成
	/// </summary>
	/// <returns>スカイボックス</returns>
	static Skybox* Create();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera& camera);

	//スケールを取得・設定
	const Vector3& GetScale() const { return worldTransform_.scale_; };
	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; };

	//色を取得・設定
	const Vector4& GetColor() const { return color_; };
	void SetColor(const Vector4& color) { color_ = color; };

	//テクスチャを設定
	void SetTexture(const std::string& textureName);

private:
	/// <summary>
	/// スカイボックスを内部で生成
	/// </summary>
	void CreateInternal();

	/// <summary>
	/// 頂点バッファを作成
	/// </summary>
	void CreateVertexBuffer();

	/// <summary>
	/// インデックスバッファを作成
	/// </summary>
	void CreateIndexBuffer();

	/// <summary>
	/// マテリアル用のリソースを作成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// マテリアル用のリソースの更新
	/// </summary>
	void UpdateMaterialResource();

private:
	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	std::unique_ptr<UploadBuffer> indexBuffer_ = nullptr;

	std::unique_ptr<UploadBuffer> materialConstBuffer_ = nullptr;

	std::array<VertexDataPosUV, kMaxVertices> vertices_{};

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::array<uint32_t, kMaxIndices> indices_{};

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	WorldTransform worldTransform_{};

	Vector4 color_{ 1.0f,1.0f,1.0f,1.0f };

	const Texture* texture_ = nullptr;
};


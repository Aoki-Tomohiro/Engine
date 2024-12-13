/**
 * @file Sprite.h
 * @brief スプライトを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/Texture.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <array>
#include <memory>
#include <string>

class Sprite
{
public:
	//頂点の最大数
	static const uint32_t kMaxVertices = 6;

	/// <summary>
	/// スプライトの生成
	/// </summary>
	/// <param name="textureName">テクスチャの名前</param>
	/// <param name="position">座標</param>
	/// <returns>スプライト</returns>
	static Sprite* Create(const std::string& textureName, Vector2 position);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//座標を取得・設定
	const Vector2& GetPosition() const { return position_; };
	void SetPosition(const Vector2& position) { position_ = position; };

	//回転を取得・設定
	float GetRotation() const { return rotation_; };
	void SetRotation(float rotation) { rotation_ = rotation; };

	//スケールを取得・設定
	const Vector2& GetScale() const { return scale_; };
	void SetScale(const Vector2& scale) { scale_ = scale; };

	//色を取得・設定
	const Vector4& GetColor() const { return color_; };
	void SetColor(const Vector4& color) { color_ = color; };

	//サイズを取得・設定
	const Vector2& GetSize() const { return size_; };
	void SetSize(const Vector2& size) { size_ = size; };

	//アンカーポイントを取得・設定
	const Vector2& GetAnchorPoint() const { return anchorPoint_; };
	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; };

	//フリップのX軸を取得・設定
	bool GetIsFlipX() const { return isFlipX_; };
	void SetIsFlipX(bool isFlipX) { isFlipX_ = isFlipX; };

	//フリップのY軸を取得・設定
	bool GetIsFlipY() const { return isFlipY_; };
	void SetIsFlipY(bool isFlipY) { isFlipY_ = isFlipY; };

	//描画フラグを取得・設定
	bool GetIsInvisible() const { return isInvisible_; };
	void SetIsInvisible(bool isInvisible) { isInvisible_ = isInvisible; };

	//テクスチャの左上を取得・設定
	const Vector2& GetTextureLeftTop() const { return textureLeftTop_; };
	void SetTextureLeftTop(const Vector2& textureLeftTop) { textureLeftTop_ = textureLeftTop; };

	//テクスチャのサイズを取得・設定
	const Vector2& GetTextureSize() const { return textureSize_; };
	void SetTextureSize(const Vector2& textureSize) { textureSize_ = textureSize; };

	//UV座標を取得・設定
	const Vector2 GetUVTranslation() const { return uvTranslation_; };
	void SetUVTranslation(const Vector2& uvTranslation) { uvTranslation_ = uvTranslation; };

	//UVの角度を取得・設定
	float GetUVRotation() const { return uvRotation_; };
	void SetUVRotation(float uvRotation) { uvRotation_ = uvRotation; };

	//UVのスケールを取得・設定
	const Vector2 GetUVScale() const { return uvScale_; };
	void SetUVScale(const Vector2& uvScale) { uvScale_ = uvScale; };

	//テクスチャを設定
	void SetTexture(const std::string& textureName);

private:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="textureName">テクスチャの名前</param>
	/// <param name="position">座標</param>
	void Initialize(const std::string& textureName, Vector2 position);

	/// <summary>
	/// 頂点バッファを作成
	/// </summary>
	void CreateVertexBuffer();

	/// <summary>
	/// 頂点バッファの更新
	/// </summary>
	void UpdateVertexBuffer();

	/// <summary>
	/// マテリアル用のリソースの作成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// マテリアル用のリソースの更新
	/// </summary>
	void UpdateMaterialResource();

	/// <summary>
	/// WVP用のリソースの作成
	/// </summary>
	void CreateWVPResource();

	/// <summary>
	/// WVP用のリソースの更新
	/// </summary>
	void UpdateWVPResource();

	/// <summary>
	/// テクスチャサイズを取得
	/// </summary>
	void AdjustTextureSize();

private:
	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	std::unique_ptr<UploadBuffer> materialConstBuffer_ = nullptr;

	std::unique_ptr<UploadBuffer> wvpResource_ = nullptr;

	std::array<VertexDataPosUV, kMaxVertices> vertices_{};

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	D3D12_RESOURCE_DESC resourceDesc_{};

	Vector2 position_ = { 0.0f,0.0f };

	float rotation_ = 0.0f;

	Vector2 scale_ = { 1.0f,1.0f };

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	Vector2 size_ = { 100.0f,100.0f };

	Vector2 anchorPoint_{ 0.0f,0.0f };

	bool isFlipX_ = false;

	bool isFlipY_ = false;

	bool isInvisible_ = false;

	Vector2 textureLeftTop_ = { 0.0f,0.0f };

	Vector2 textureSize_ = { 100.0f,100.0f };

	Vector2 uvTranslation_ = { 0.0f,0.0f };

	float uvRotation_ = 0.0f;

	Vector2 uvScale_ = { 1.0f,1.0f };

	const Texture* texture_ = nullptr;
};


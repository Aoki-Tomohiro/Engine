/**
 * @file GaussianBlur.h
 * @brief ガウシアンブラーを管理を行うクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/ColorBuffer.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <array>
#include <memory>

class GaussianBlur
{
public:
	//ブラーを掛ける方向
	enum BlurDirection
	{
		kHorizontal,
		kVertical,
		kCountOfBlurDirection
	};

	/// <summary>
	/// 静的初期化処理
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// 破棄処理
	/// </summary>
	static void Destroy();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="width">テクスチャの横幅</param>
	/// <param name="height">テクスチャの立幅</param>
	void Initialize(uint32_t width, uint32_t height);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ブラーをかける前の処理
	/// </summary>
	/// <param name="index">ブラーのインデックス</param>
	void PreBlur(uint32_t index);

	/// <summary>
	/// ブラーを掛けた後の処理
	/// </summary>
	/// <param name="index">ブラーのインデックス</param>
	void PostBlur(uint32_t index);

	//テクスチャの横幅を取得・設定
	const uint32_t GetTextureWidth() const { return textureWidth_; };
	void SetTextureWidth(const uint32_t textureWidth) { textureWidth_ = textureWidth; };

	//テクスチャの立幅を取得・設定
	const uint32_t GetTextureHeight() const { return textureHeight_; };
	void SetTextureHeight(const uint32_t textureHeight) { textureHeight_ = textureHeight; };

	//シグマを取得・設定
	const float GetSigma() const { return sigma_; };
	void SetSigma(const float sigma) { sigma_ = sigma; };

	//コンスタントバッファを取得
	const UploadBuffer* GetConstantBuffer() const { return constBuff_.get(); };

	//デスクリプタハンドルを取得
	const DescriptorHandle& GetDescriptorHandle(uint32_t index) const { return blurBuffers_[index]->GetSRVHandle(); };

private:
	static std::unique_ptr<RootSignature> rootSignature_;

	static std::array<std::unique_ptr<GraphicsPSO>, kCountOfBlurDirection> pipelineStates_;

	std::array<std::unique_ptr<ColorBuffer>, kCountOfBlurDirection> blurBuffers_{};

	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

	uint32_t textureWidth_ = 0;

	uint32_t textureHeight_ = 0;

	float sigma_ = 5.0f;
};


/**
 * @file Bloom.h
 * @brief ブルームの管理を行うクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "GaussianBlur.h"

class Bloom
{
public:
	//ブラーを掛ける最大数
	static const int kMaxBlurCount = 4;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// Bloomを適用
	/// </summary>
	/// <param name="srvHandle">Srvのハンドル</param>
	void Apply(const DescriptorHandle& srvHandle);

	//ブラー回数を取得・設定
	const uint32_t GetBlurCount() const { return blurCount_; };
	void SetBlurCount(const uint32_t blurCount) { blurCount_ = blurCount; };

	//有効にするかどうかを取得・設定
	const bool GetIsEnable() const { return isEnable_; };
	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

	//強さを取得・設定
	const float GetIntensity() const { return intensity_; };
	void SetIntensity(const float intensity) { intensity_ = intensity; };

	//テクスチャの重みを取得・設定
	const float GetTextureWeight() const { return textureWeight_; };
	void SetTextureWeight(const float textureWeight) { textureWeight_ = textureWeight; };

	//高輝度テクスチャの重みを取得・設定
	const float GetHighLumTextureWeight() const { return highLumTextureWeight_; };
	void SetHighLumTextureWeight(const float highLumTextureWeight) { highLumTextureWeight_ = highLumTextureWeight; };

	//ブラーテクスチャの重みを取得・設定
	const float GetBlurTextureWeight(const uint32_t index) const { return blurTextureWeight_[index]; };
	void SetBlurTextureWeight(const uint32_t index, const float blurTextureWeight) { blurTextureWeight_[index] = blurTextureWeight; };

	//ガウシアンブラーを取得
	GaussianBlur* GetGaussianBlur(uint32_t index) const { return gaussianBlurs_[index].get(); };

	//デスクリプタハンドルを取得
	const DescriptorHandle& GetDescriptorHandle() const { return bloomColorBuffer_->GetSRVHandle(); };

private:
	/// <summary>
	/// 高輝度のパイプラインステートを生成
	/// </summary>
	void CreateHighLumPipelineState();

	/// <summary>
	/// ブルームのパイプラインステートを生成
	/// </summary>
	void CreateBloomPipelineState();

	/// <summary>
	/// 高輝度を描画
	/// </summary>
	/// <param name="srvHandle">Srvハンドル</param>
	void RenderHighLuminance(const DescriptorHandle& srvHandle);

	/// <summary>
	/// ガウシアンブラーを適用
	/// </summary>
	void ApplyGaussianBlur();

private:
	//RootSignature
	RootSignature highLumRootSignature_{};
	RootSignature bloomRootSignature_{};

	//PipelineState
	GraphicsPSO highLumPipelineState_{};
	GraphicsPSO bloomPipelineState_{};

	//ColorBuffer
	std::unique_ptr<ColorBuffer> highLumColorBuffer_ = nullptr;
	std::unique_ptr<ColorBuffer> bloomColorBuffer_ = nullptr;

	//GaussianBlur
	std::array<std::unique_ptr<GaussianBlur>, kMaxBlurCount> gaussianBlurs_{};

	//ConstBuffer
	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

	//調整項目
	int32_t isEnable_ = false;

	float intensity_ = 1.0f;

	float textureWeight_ = 1.0f;

	float highLumTextureWeight_ = 1.0f;

	float blurTextureWeight_[4] = { 1.0f,1.0f,1.0f,1.0f };

	uint32_t blurCount_ = kMaxBlurCount;
};


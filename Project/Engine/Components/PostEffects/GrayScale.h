/**
 * @file GrayScale.h
 * @brief グレースケールを管理を行うクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/ColorBuffer.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"

class GrayScale
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// グレースケールを適用
	/// </summary>
	/// <param name="srvHandle">Srvハンドル</param>
	void Apply(const DescriptorHandle& srvHandle);

	//有効にするかどうかを取得・設定
	const bool GetIsEnable() const { return isEnable_; };
	void SetIsEnable(const int32_t isEnable) { isEnable_ = isEnable; };

	//セピア長にするかどうかを取得・設定
	const bool GetIsSepiaEnabled() const { return isSepiaEnabled_; };
	void SetIsSepiaEnabled(const int32_t isSepiaEnabled) { isSepiaEnabled_ = isSepiaEnabled; };

	//デスクリプタハンドルを取得
	const DescriptorHandle& GetDescriptorHandle() const { return colorBuffer_->GetSRVHandle(); };

private:
	/// <summary>
	/// パイプラインステートを生成
	/// </summary>
	void CreatePipelineState();

private:
	//RootSignature
	RootSignature rootSignature_{};

	//PipelineState
	GraphicsPSO pipelineState_{};

	//ColorBuffer
	std::unique_ptr<ColorBuffer> colorBuffer_ = nullptr;

	//ConstBuffer
	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

	//調整項目
	int32_t isEnable_ = false;

	int32_t isSepiaEnabled_ = false;
};


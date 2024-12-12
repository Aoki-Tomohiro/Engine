/**
 * @file LensDistortion.h
 * @brief レンズディストーションを管理するクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/ColorBuffer.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"

class LensDistortion
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
	/// レンズディストーションを適用
	/// </summary>
	/// <param name="srvHandle">Srvハンドル</param>
	void Apply(const DescriptorHandle& srvHandle);

	//有効にするかどうかを取得・設定
	const bool GetIsEnable() const { return isEnable_; };
	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

	//タイトレスの取得・設定
	const float GetTightness() const { return tightness_; };
	void SetTightness(const float tightness) { tightness_ = tightness; };

	//強さの取得・設定
	const float GetStrength() const { return strength_; };
	void SetStrength(const float strength) { strength_ = strength; };

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
	bool isEnable_ = false;

	float tightness_ = 2.5f;

	float strength_ = -0.1f;
};


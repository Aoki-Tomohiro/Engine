/**
 * @file HSV.h
 * @brief HSVフィルターを管理するクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <memory>

class HSV
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

	//色相の取得・設定
	const float GetHue() const { return hue_; };
	void SetHue(const float hue) { hue_ = hue; };

	//彩度の取得・設定
	const float GetSaturation() const { return saturation_; };
	void SetSaturation(const float saturation) { saturation_ = saturation; };

	//明度の取得・設定
	const float GetValue() const { return value_; };
	void SetValue(const float value) { value_ = value; };

	//コンスタントバッファを取得
	const UploadBuffer* GetConstantBuffer() const { return constBuff_.get(); };

private:
	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

	float hue_ = 0.0f;

	float saturation_ = 0.0f;

	float value_ = 0.0f;
};


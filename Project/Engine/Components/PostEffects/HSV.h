#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <memory>

class HSV
{
public:
	void Initialize();

	void Update();

	const float GetHue() const { return hue_; };

	void SetHue(const float hue) { hue_ = hue; };

	const float GetSaturation() const { return saturation_; };

	void SetSaturation(const float saturation) { saturation_ = saturation; };

	const float GetValue() const { return value_; };

	void SetValue(const float value) { value_ = value; };

	const UploadBuffer* GetConstantBuffer() const { return constBuff_.get(); };

private:
	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

	float hue_ = 0.0f;

	float saturation_ = 0.0f;

	float value_ = 0.0f;
};


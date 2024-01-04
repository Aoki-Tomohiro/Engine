#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <memory>

class Bloom
{
public:
	void Initialize();

	void Update();

	const UploadBuffer* GetConstBuffer() const { return constBuff_.get(); };

	const bool GetIsEnable() const { return isEnable_; };

	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

	const float GetIntensity() const { return intensity_; };

	void SetIntensity(const float intensity) { intensity_ = intensity; };

	const float GetHighIntensityColorWeight() const { return highIntensityColorWeight_; };

	void SetHighIntensityColorWeight(const float highIntensityColorWeight) { highIntensityColorWeight_ = highIntensityColorWeight; };

	const float GetHighIntensityBlurColorWeight() const { return highIntensityBlurColorWeight_; };

	void SetHighIntensityBlurColorWeight(const float highIntensityBlurColorWeight) { highIntensityBlurColorWeight_ = highIntensityBlurColorWeight; };

	const float GetHighIntensityShrinkBlurColorWeight() const { return highIntensityShrinkBlurColorWeight_; };

	void SetHighIntensityShrinkBlurColorWeight(const float highIntensityShrinkBlurColorWeight) { highIntensityShrinkBlurColorWeight_ = highIntensityShrinkBlurColorWeight; };

private:
	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

	int32_t isEnable_ = false;

	float intensity_ = 1.0f;

	float highIntensityColorWeight_ = 1.0f;

	float highIntensityBlurColorWeight_ = 0.5f;

	float highIntensityShrinkBlurColorWeight_ = 0.2f;
};


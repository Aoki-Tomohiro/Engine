#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <memory>

class Fog
{
public:
	void Initialize();

	void Update();

	const UploadBuffer* GetConstBuffer() const { return constBuff_.get(); };

	const bool GetIsEnable() const { return isEnable_; };

	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

	const float GetScale() const { return scale_; };

	void SetScale(const float scale) { scale_ = scale; };

	const float GetAttenuationRate() { return attenuationRate_; };

	void SetAttenuationRate(const float attenuationRate) { attenuationRate_ = attenuationRate; };

private:
	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

	bool isEnable_ = false;

	float scale_ = 0.5f;

	float attenuationRate_ = 2.0f;
};


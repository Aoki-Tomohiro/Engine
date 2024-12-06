#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/ColorBuffer.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"

class Vignette
{
public:
	void Initialize();

	void Update();

	void Apply(const DescriptorHandle& srvHandle);

	const bool GetIsEnable() const { return isEnable_; };

	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

	const float GetScale() const { return scale_; };

	void SetScale(const float scale) { scale_ = scale; };

	const float GetIntensity() const { return intensity_; };

	void SetIntensity(const float intensity) { intensity_ = intensity; };

	const DescriptorHandle& GetDescriptorHandle() const { return colorBuffer_->GetSRVHandle(); };

private:
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

	bool isEnable_ = false;

	float scale_ = 16.0f;

	float intensity_ = 0.2f;
};


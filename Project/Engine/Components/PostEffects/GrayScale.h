#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/ColorBuffer.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"

class GrayScale
{
public:
	void Initialize();

	void Update();

	void Apply(const DescriptorHandle& srvHandle);

	const bool GetIsEnable() const { return isEnable_; };

	void SetIsEnable(const int32_t isEnable) { isEnable_ = isEnable; };

	const bool GetIsSepiaEnabled() const { return isSepiaEnabled_; };

	void SetIsSepiaEnabled(const int32_t isSepiaEnabled) { isSepiaEnabled_ = isSepiaEnabled; };

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

	//調整項目
	int32_t isEnable_ = false;

	int32_t isSepiaEnabled_ = false;
};


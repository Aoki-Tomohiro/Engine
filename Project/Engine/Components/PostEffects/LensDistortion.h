#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/ColorBuffer.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"

class LensDistortion
{
public:
	void Initialize();

	void Update();

	void Apply(const DescriptorHandle& srvHandle);

	const bool GetIsEnable() const { return isEnable_; };

	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

	const float GetTightness() const { return tightness_; };

	void SetTightness(const float tightness) { tightness_ = tightness; };

	const float GetStrength() const { return strength_; };

	void SetStrength(const float strength) { strength_ = strength; };

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
	bool isEnable_ = false;

	float tightness_ = 2.5f;

	float strength_ = -0.1f;
};


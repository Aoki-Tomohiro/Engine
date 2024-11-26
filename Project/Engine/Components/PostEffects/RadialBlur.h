#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/ColorBuffer.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"

class RadialBlur
{
public:
	void Initialize();

	void Update();

	void Apply(const DescriptorHandle& srvHandle);

	const bool GetIsEnable() const { return isEnable_; };

	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

	const Vector2& GetCenter() const { return center_; };

	void SetCenter(const Vector2& center) { center_ = center; };

	const float GetBlurWidth() const { return blurWidth_; };

	void SetBlurWidth(const float blurWidth) { blurWidth_ = blurWidth; };

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

	Vector2 center_ = { 0.5f,0.5f };

	float blurWidth_ = 0.04f;
};


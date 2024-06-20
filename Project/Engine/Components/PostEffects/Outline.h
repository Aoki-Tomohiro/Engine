#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/ColorBuffer.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"

class Outline
{
public:
	void Initialize();

	void Update();

	void Apply(const DescriptorHandle& srvHandle);

	const bool GetIsEnable() const { return isEnable_; };

	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

	const Matrix4x4& GetProjectionInverse() const { return projectionInverse_; };

	void SetProjectionInverse(const Matrix4x4& projectionInverse) { projectionInverse_ = projectionInverse; };

	const float GetCoefficient() const { return coefficient_; };

	void SetCoefficient(const float coefficient) { coefficient_ = coefficient; };

	const UploadBuffer* GetConstBuffer() const { return constBuff_.get(); };

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

	//逆プロジェクション行列
	Matrix4x4 projectionInverse_{};

	//係数
	float coefficient_ = 1.0f;
};


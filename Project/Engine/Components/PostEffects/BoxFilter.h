#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <memory>


class BoxFilter
{
public:
	void Initialize();

	void Update();

	const UploadBuffer* GetConstBuffer() const { return constBuff_.get(); };

	const bool GetIsEnable() const { return isEnable_; };

	void SetIsEnable(const int32_t isEnable) { isEnable_ = isEnable; };

	const int32_t GetKernelSize() const { return kernelSize_; };

	void SetKernelSize(const int32_t kernelSize) { kernelSize_ = kernelSize; };

private:
	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

	int32_t isEnable_ = false;

	int32_t kernelSize_ = KernelSize::k3x3;
};


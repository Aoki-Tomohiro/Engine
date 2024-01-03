#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <memory>

class DepthofField
{
public:
	void Initialize();

	void Update();

	const UploadBuffer* GetConstBuffer() const { return constBuff_.get(); };

	const bool GetIsEnable() const { return isEnable_; };

	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

private:
	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

	bool isEnable_ = false;
};


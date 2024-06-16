#pragma once
#include "GpuResource.h"
#include "DescriptorHandle.h"
#include <cstdint>

class RWStructuredBuffer : public GpuResource
{
public:
	void Create(uint32_t numElements, uint32_t elementSize);

	const DescriptorHandle& GetUAVHandle() { return uavHandle_; };

	size_t GetBufferSize() const { return bufferSize_; };

private:
	void CreateDerivedViews(ID3D12Device* device, uint32_t numElements, uint32_t elementSize);

private:
	DescriptorHandle uavHandle_{};

	size_t bufferSize_ = 0;

	uint32_t elementCount_ = 0;

	uint32_t elementSize_ = 0;
};


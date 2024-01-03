#pragma once
#include "GpuResource.h"
#include <cstdint>

class StructuredBuffer : public GpuResource
{
public:
	void Create(uint32_t numElements, uint32_t elementSize);

	void* Map();

	void Unmap();

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() { return srvHandle_; };

	size_t GetBufferSize() const { return bufferSize_; };

private:
	void CreateDerivedViews(ID3D12Device* device, uint32_t numElements, uint32_t elementSize);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_{};

	size_t bufferSize_ = 0;

	uint32_t elementCount_ = 0;

	uint32_t elementSize_ = 0;
};


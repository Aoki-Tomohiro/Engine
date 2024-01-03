#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <vector>

class DescriptorAllocator
{
public:
	static void DestroyAll();

	DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type) : heapType_(type), descriptorSize_(0) {};

	D3D12_CPU_DESCRIPTOR_HANDLE Allocate();

private:
	static ID3D12DescriptorHeap* RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);

private:
	static const uint32_t kNumDescriptors = 256;

	static std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> descriptorHeapPool_;

	D3D12_DESCRIPTOR_HEAP_TYPE heapType_;

	ID3D12DescriptorHeap* currentHeap_ = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE currentHandle_{};

	uint32_t descriptorSize_ = 0;

	uint32_t freeHandles_ = 0;
};


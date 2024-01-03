#include "DescriptorAllocator.h"
#include "GraphicsCore.h"

std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> DescriptorAllocator::descriptorHeapPool_;

void DescriptorAllocator::DestroyAll()
{
	descriptorHeapPool_.clear();
}

ID3D12DescriptorHeap* DescriptorAllocator::RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = type;
	heapDesc.NumDescriptors = kNumDescriptors;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&descriptorHeap));
	descriptorHeapPool_.emplace_back(descriptorHeap);
	return descriptorHeap.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::Allocate()
{
	if (currentHeap_ == nullptr || freeHandles_ < 0)
	{
		currentHeap_ = RequestNewHeap(heapType_);
		currentHandle_ = currentHeap_->GetCPUDescriptorHandleForHeapStart();
		freeHandles_ = kNumDescriptors;

		if (descriptorSize_ == 0)
		{
			descriptorSize_ = GraphicsCore::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(heapType_);
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE ret = currentHandle_;
	currentHandle_.ptr += descriptorSize_;
	freeHandles_--;
	return ret;
}
#pragma once
#include "PSO.h"

class CommandContext
{
public:
	~CommandContext() = default;

	void Initialize();

	void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* descriptorHeap);

	void SetPipelineState(const PSO& pipelineState);

	void Close();

	void Reset();

	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); };

private:
	void BindDescriptorHeaps();

protected:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	ID3D12RootSignature* currentRootSignature_ = nullptr;

	ID3D12PipelineState* currentPipelineState_ = nullptr;

	ID3D12DescriptorHeap* currentDescriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
};


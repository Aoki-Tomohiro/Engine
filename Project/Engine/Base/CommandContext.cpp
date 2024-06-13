#include "CommandContext.h"
#include "GraphicsCore.h"

void CommandContext::Initialize()
{
	//デバイスを取得
	ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();

	//コマンドアロケータを作成する
	HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	//コマンドアロケーターの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドリストを作成する
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	//コマンドリストの作成がうまくできなかったので起動できない
	assert(SUCCEEDED(hr));
}

void CommandContext::SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* descriptorHeap)
{
	if (currentDescriptorHeaps_[type] != descriptorHeap)
	{
		currentDescriptorHeaps_[type] = descriptorHeap;
		BindDescriptorHeaps();
	}
}

void CommandContext::SetPipelineState(const PSO& pipelineState)
{
	if (pipelineState.GetPipelineState() == currentPipelineState_)
	{
		return;
	}
	commandList_->SetPipelineState(currentPipelineState_ = pipelineState.GetPipelineState());
}

void CommandContext::Close()
{
	HRESULT hr = commandList_->Close();
	assert(SUCCEEDED(hr));
}

void CommandContext::Reset()
{
	HRESULT hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));

	if (currentRootSignature_)
	{
		commandList_->SetGraphicsRootSignature(currentRootSignature_);
	}

	if (currentPipelineState_)
	{
		commandList_->SetPipelineState(currentPipelineState_);
	}

	BindDescriptorHeaps();
}

void CommandContext::BindDescriptorHeaps()
{
	UINT nonNullHeaps = 0;
	ID3D12DescriptorHeap* heapsToBind[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
	for (UINT i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
	{
		ID3D12DescriptorHeap* heapIter = currentDescriptorHeaps_[i];
		if (heapIter != nullptr)
		{
			heapsToBind[nonNullHeaps++] = heapIter;
		}
	}

	if (nonNullHeaps > 0)
	{
		commandList_->SetDescriptorHeaps(nonNullHeaps, heapsToBind);
	}
}
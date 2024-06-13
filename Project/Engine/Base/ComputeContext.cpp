#include "ComputeContext.h"

void ComputeContext::SetRootSignature(const RootSignature& rootSignature)
{
	if (rootSignature.GetRootSignature() == currentRootSignature_)
	{
		return;
	}
	commandList_->SetGraphicsRootSignature(currentRootSignature_ = rootSignature.GetRootSignature());
}

void ComputeContext::SetDescriptorTable(UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
{
	commandList_->SetComputeRootDescriptorTable(rootParameterIndex, gpuHandle);
}

void ComputeContext::SetConstantBuffer(UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS cbv)
{
	commandList_->SetGraphicsRootConstantBufferView(rootParameterIndex, cbv);
}

void ComputeContext::Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ)
{
	commandList_->Dispatch((UINT)groupCountX, (UINT)groupCountY, (UINT)groupCountZ);
}
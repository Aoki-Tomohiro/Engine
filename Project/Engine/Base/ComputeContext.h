#pragma once
#include "CommandContext.h"
#include "RootSignature.h"

class ComputeContext : public CommandContext
{
public:
	void SetRootSignature(const RootSignature& rootSignature);

	void SetDescriptorTable(UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

	void SetConstantBuffer(UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS cbv);

	void Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ);
};


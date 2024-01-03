#pragma once
#include <d3d12.h>
#include <wrl.h>

class GpuResource
{
	friend class CommandContext;

public:
	virtual ~GpuResource() = default;

	ID3D12Resource* GetResource() const { return resource_.Get(); };

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return gpuVirtualAddress_; };

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

	D3D12_RESOURCE_STATES currentState_{};

	D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress_ = 0;
};


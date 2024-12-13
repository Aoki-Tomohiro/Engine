/**
 * @file GpuResource.h
 * @brief GPUで使うリソースを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include <d3d12.h>
#include <wrl.h>

class GpuResource
{
	friend class CommandContext;

public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~GpuResource() = default;

	//リソースを取得
	ID3D12Resource* GetResource() const { return resource_.Get(); };

	//GPUアドレスを取得
	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return gpuVirtualAddress_; };

	//リソースの状態を取得・設定
	D3D12_RESOURCE_STATES GetResourceState() { return currentState_; };
	void SetResourceState(D3D12_RESOURCE_STATES newState) { currentState_ = newState; };

protected:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

	D3D12_RESOURCE_STATES currentState_{};

	D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress_ = 0;
};


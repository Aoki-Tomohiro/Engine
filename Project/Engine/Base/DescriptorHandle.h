/**
 * @file DescriptorHandle.h
 * @brief デスクリプタハンドルを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include <d3d12.h>

class DescriptorHandle
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DescriptorHandle() = default;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="cpuHandle">CPUハンドル</param>
	/// <param name="gpuHandle">GPUハンドル</param>
	DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) : cpuHandle_(cpuHandle), gpuHandle_(gpuHandle) {};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="cpuHandle">CPUハンドル</param>
	DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle) : cpuHandle_(cpuHandle) {};

	//指定されたディスクリプタサイズ分だけデスクリプタハンドルを移動
	void operator+=(UINT descriptorSize)
	{
		cpuHandle_.ptr += descriptorSize;
		gpuHandle_.ptr += descriptorSize;
	}

	//CPUハンドルへのポインタを返す
	const D3D12_CPU_DESCRIPTOR_HANDLE* operator&() const { return &cpuHandle_; }

	//CPUハンドルを返す
	operator D3D12_CPU_DESCRIPTOR_HANDLE() const { return cpuHandle_; };

	//GPUハンドルを返す
	operator D3D12_GPU_DESCRIPTOR_HANDLE() const { return gpuHandle_; };

private:
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_{};

	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_{};
};


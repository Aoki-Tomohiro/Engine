/**
 * @file DescriptorHeap.h
 * @brief デスクリプタヒープを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "DescriptorHandle.h"
#include <cstdint>
#include <wrl.h>

class DescriptorHeap
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="type">デスクリプタヒープの種類</param>
	/// <param name="numDescriptors">デスクリプタの数</param>
	/// <param name="shaderVisible">シェーダーで使うかどうか</param>
	void Initialize(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool shaderVisible);

	/// <summary>
	/// デスクリプタハンドルを割り当てる
	/// </summary>
	/// <returns>割り当てたデスクリプタハンドル</returns>
	DescriptorHandle Allocate();

	//デスクリプタのサイズを取得
	uint32_t GetDescriptorSize() const { return descriptorSize_; }

	//デスクリプタヒープを取得
	ID3D12DescriptorHeap* GetDescriptorHeap() const { return descriptorHeap_.Get(); };

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;

	UINT descriptorSize_ = 0;

	uint32_t numFreeDescriptors_ = 0;

	DescriptorHandle firstHandle_;

	DescriptorHandle nextFreeHandle_;
};


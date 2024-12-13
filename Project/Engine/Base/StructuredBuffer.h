/**
 * @file StructuredBuffer.h
 * @brief StructuredBufferを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "GpuResource.h"
#include "DescriptorHandle.h"
#include <cstdint>

class StructuredBuffer : public GpuResource
{
public:
	/// <summary>
	/// StructuredBufferを作成
	/// </summary>
	/// <param name="numElements">属性の数</param>
	/// <param name="elementSize">属性のサイズ</param>
	void Create(uint32_t numElements, uint32_t elementSize);

	/// <summary>
	/// リソースに書き込む
	/// </summary>
	/// <returns>バイナリデータ</returns>
	void* Map();

	/// <summary>
	/// 書き込みをやめる
	/// </summary>
	void Unmap();

	//SRVハンドルを取得
	const DescriptorHandle& GetSRVHandle() { return srvHandle_; };

	//バッファサイズを取得
	size_t GetBufferSize() const { return bufferSize_; };

private:
	/// <summary>
	/// ビューの作成
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="numElements">属性の数</param>
	/// <param name="elementSize">属性のサイズ</param>
	void CreateDerivedViews(ID3D12Device* device, uint32_t numElements, uint32_t elementSize);

private:
	DescriptorHandle srvHandle_{};

	size_t bufferSize_ = 0;

	uint32_t elementCount_ = 0;

	uint32_t elementSize_ = 0;
};


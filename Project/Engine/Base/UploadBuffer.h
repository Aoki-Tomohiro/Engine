/**
 * @file UploadBuffer.h
 * @brief アップロードバッファを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "GpuResource.h"

class UploadBuffer : public GpuResource
{
public:
	/// <summary>
	/// アップロードバッファを作成
	/// </summary>
	/// <param name="sizeInBytes">バッファのサイズ</param>
	void Create(size_t sizeInBytes);

	/// <summary>
	/// リソースに書き込む
	/// </summary>
	/// <returns>バイナリデータ</returns>
	void* Map();

	/// <summary>
	/// 書き込みをやめる
	/// </summary>
	void Unmap();

	//バッファのサイズを取得
	size_t GetBufferSize() const { return bufferSize_; };

private:
	size_t bufferSize_ = 0;
};


/**
 * @file DepthBuffer.h
 * @brief 深度バッファを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "GpuResource.h"
#include "DescriptorHandle.h"
#include <cstdint>

class DepthBuffer : public GpuResource
{
public:
	/// <summary>
	/// 深度バッファを作成
	/// </summary>
	/// <param name="width">リソースの横幅</param>
	/// <param name="height">リソースの縦幅</param>
	/// <param name="format">フォーマット</param>
	void Create(uint32_t width, uint32_t height, DXGI_FORMAT format);

	/// <summary>
	/// 深度バッファを作成
	/// </summary>
	/// <param name="width">リソースの横幅</param>
	/// <param name="height">リソースの縦幅</param>
	/// <param name="format">フォーマット</param>
	/// <param name="useAsTexture">テクスチャとして使うか</param>
	void Create(uint32_t width, uint32_t height, DXGI_FORMAT format, bool useAsTexture);

	//Dsvハンドルを取得
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSVHandle() const { return dsvHandle_; };

	//Srvハンドルを取得
	const DescriptorHandle& GetSRVHandle() const { return srvHandle_; };

private:
	/// <summary>
	/// ビューの作成
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="format">フォーマット</param>
	void CreateDerivedViews(ID3D12Device* device, DXGI_FORMAT format);

private:
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_{};

	DescriptorHandle srvHandle_{};

	bool useAsTexture_ = false;
};


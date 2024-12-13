/**
 * @file ColorBuffer.h
 * @brief カラーバッファを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "GpuResource.h"
#include "DescriptorHandle.h"
#include <cstdint>

class ColorBuffer : public GpuResource
{
public:
	/// <summary>
	/// スワップチェーンからカラーバッファを作成
	/// </summary>
	/// <param name="baseResource">元のリソース</param>
	void CreateFromSwapChain(ID3D12Resource* baseResource);

	/// <summary>
	/// カラーバッファを作成
	/// </summary>
	/// <param name="width">リソースの横幅</param>
	/// <param name="height">リソースの縦幅</param>
	/// <param name="format">フォーマット</param>
	void Create(uint32_t width, uint32_t height, DXGI_FORMAT format);

	/// <summary>
	/// カラーバッファを作成
	/// </summary>
	/// <param name="width">リソースの横幅</param>
	/// <param name="height">リソースの縦幅</param>
	/// <param name="format">フォーマット</param>
	/// <param name="clearColor">画面のクリアカラー</param>
	void Create(uint32_t width, uint32_t height, DXGI_FORMAT format, float* clearColor);

	//Rtvハンドルを取得
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandle() const { return rtvHandle_; };

	//Srvハンドルを取得
	const DescriptorHandle& GetSRVHandle() const { return srvHandle_; };

	//クリアカラーを取得
	const float* GetClearColor() const { return clearColor_; };

private:
	/// <summary>
	/// ビューの作成
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="format">フォーマット</param>
	void CreateDerivedViews(ID3D12Device* device, DXGI_FORMAT format);

private:
	DescriptorHandle srvHandle_{};

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_{};

	float clearColor_[4]{ 0.1f, 0.25f, 0.5f, 1.0f };
};


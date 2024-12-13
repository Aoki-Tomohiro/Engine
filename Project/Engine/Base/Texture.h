/**
 * @file Texture.h
 * @brief テクスチャを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "GpuResource.h"
#include "DescriptorHandle.h"
#include "Engine/Externals/DirectXTex/DirectXTex.h"
#include "Engine/Externals/DirectXTex/d3dx12.h"
#include <cstdint>
#include <string>

class Texture : public GpuResource
{
public:
	/// <summary>
	/// テクスチャの作成
	/// </summary>
	/// <param name="mipImages">ミップイメージ</param>
	void Create(const DirectX::ScratchImage& mipImages);

	//Srvハンドル
	const DescriptorHandle& GetSRVHandle() const { return srvHandle_; }

	//リソースの設定
	const D3D12_RESOURCE_DESC& GetResourceDesc() const { return resourceDesc_; };

private:
	/// <summary>
	/// ビューの作成
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="metadata">メタデータ</param>
	void CreateDerivedViews(ID3D12Device* device, const DirectX::TexMetadata& metadata);

	/// <summary>
	/// テクスチャデータを転送
	/// </summary>
	/// <param name="texture">テクスチャのリソース</param>
	/// <param name="mipImages">ミップイメージ</param>
	void UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);

private:
	D3D12_RESOURCE_DESC resourceDesc_{};

	DescriptorHandle srvHandle_{};
};


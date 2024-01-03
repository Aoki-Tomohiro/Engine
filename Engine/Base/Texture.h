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
	void Create(const std::string& filePath);

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRV() const { return cpuHandle_; }

	const D3D12_RESOURCE_DESC& GetResourceDesc() const { return resourceDesc_; };

private:
	void CreateResource(const DirectX::TexMetadata& metadata);

	void UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);
	
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

private:
	D3D12_RESOURCE_DESC resourceDesc_{};

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_{};

	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = nullptr;
};


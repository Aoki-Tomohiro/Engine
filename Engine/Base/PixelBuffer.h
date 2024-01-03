#pragma once
#include "GpuResource.h"
#include <cstdint>

class PixelBuffer : public GpuResource 
{
public:
	~PixelBuffer() override = default;

	uint32_t GetWidth() const { return width_; };

	uint32_t GetHeight() const { return height_; };

	const DXGI_FORMAT& GetFormat() const { return format_; }

protected:
	D3D12_RESOURCE_DESC DescribeTex2D(uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flag);

	void AssociateWithResource(ID3D12Device* device, ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState);

	void CreateTextureResource(ID3D12Device* device, const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE& clearValue);

protected:
	uint32_t width_ = 0;

	uint32_t height_ = 0;

	DXGI_FORMAT format_{};
};


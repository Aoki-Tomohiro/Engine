#pragma once
#include "PixelBuffer.h"
#include "Color.h"

class ColorBuffer : public PixelBuffer
{
public:
	void CreateFromSwapChain(ID3D12Resource* baseResource);

	void Create(uint32_t width, uint32_t height, DXGI_FORMAT format);

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandle() const { return rtvHandle_; };

	const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRVHandle() const { return srvHandle_; };

	Color GetClearColor() const { return clearColor_; };

private:
	void CreateDerivedViews(ID3D12Device* device, DXGI_FORMAT format);

private:
	Color clearColor_{};

	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_{};

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_{};
};


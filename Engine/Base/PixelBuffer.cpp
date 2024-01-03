#include "PixelBuffer.h"
#include <cassert>

D3D12_RESOURCE_DESC PixelBuffer::DescribeTex2D(uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_RESOURCE_FLAGS flag)
{
	width_ = width;
	height_ = height;
	format_ = format;

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Flags = flag;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	return resourceDesc;
}

void PixelBuffer::AssociateWithResource(ID3D12Device* device, ID3D12Resource* resource, D3D12_RESOURCE_STATES currentState)
{
	D3D12_RESOURCE_DESC resourceDesc = resource->GetDesc();

	resource_.Attach(resource);
	currentState_ = currentState;

	width_ = uint32_t(resourceDesc.Width);
	height_ = uint32_t(resourceDesc.Height);
	format_ = resourceDesc.Format;
}

void PixelBuffer::CreateTextureResource(ID3D12Device* device, const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE& clearValue)
{
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_COMMON, &clearValue,
		IID_PPV_ARGS(&resource_));
	assert(SUCCEEDED(hr));

	currentState_ = D3D12_RESOURCE_STATE_COMMON;
}
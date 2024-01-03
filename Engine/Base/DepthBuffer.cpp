#include "DepthBuffer.h"
#include "GraphicsCore.h"

void DepthBuffer::Create(uint32_t width, uint32_t height, DXGI_FORMAT format)
{
	ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();

	D3D12_RESOURCE_DESC resourceDesc = DescribeTex2D(width, height, format, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.Format = format;

	CreateTextureResource(device, resourceDesc, clearValue);
	CreateDerivedViews(device, format);
}

void DepthBuffer::Create(uint32_t width, uint32_t height, DXGI_FORMAT format, bool useAsTexture)
{
	ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();

	D3D12_RESOURCE_DESC resourceDesc = DescribeTex2D(width, height, format, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	useAsTexture_ = useAsTexture;

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.Format = format;

	CreateTextureResource(device, resourceDesc, clearValue);
	CreateDerivedViews(device, format);
}

void DepthBuffer::CreateDerivedViews(ID3D12Device* device, DXGI_FORMAT format)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = format;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	dsvHandle_ = GraphicsCore::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	device->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsvHandle_);

	if (useAsTexture_)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Format = (format == DXGI_FORMAT_D32_FLOAT) ? DXGI_FORMAT_R32_FLOAT : format;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvHandle_ = GraphicsCore::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);
	}
}
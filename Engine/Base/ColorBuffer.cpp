#include "ColorBuffer.h"
#include "GraphicsCore.h"

void ColorBuffer::CreateFromSwapChain(ID3D12Resource* baseResource)
{
	GraphicsCore* graphicsCore = GraphicsCore::GetInstance();

	ID3D12Device* device = graphicsCore->GetDevice();

	AssociateWithResource(device, baseResource, D3D12_RESOURCE_STATE_PRESENT);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvHandle_ = graphicsCore->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device->CreateRenderTargetView(resource_.Get(), &rtvDesc, rtvHandle_);
}

void ColorBuffer::Create(uint32_t width, uint32_t height, DXGI_FORMAT format)
{
	ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();

	D3D12_RESOURCE_DESC resourceDesc = DescribeTex2D(width, height, format, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format;
	clearValue.Color[0] = clearColor_.R();
	clearValue.Color[1] = clearColor_.G();
	clearValue.Color[2] = clearColor_.B();
	clearValue.Color[3] = clearColor_.A();

	CreateTextureResource(device, resourceDesc, clearValue);
	CreateDerivedViews(device, format);
}

void ColorBuffer::CreateDerivedViews(ID3D12Device* device, DXGI_FORMAT format)
{
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = format;
	rtvHandle_ = GraphicsCore::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device->CreateRenderTargetView(resource_.Get(), &rtvDesc, rtvHandle_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvHandle_ = GraphicsCore::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);
}
/**
 * @file GraphicsPSO.cpp
 * @brief パイプラインステートを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "GraphicsPSO.h"
#include "GraphicsCore.h"
#include <cassert>

void GraphicsPSO::SetInputLayout(UINT numElements, const D3D12_INPUT_ELEMENT_DESC* inputElementDescs)
{
	pipelineStateDesc_.InputLayout.pInputElementDescs = inputElementDescs;
	pipelineStateDesc_.InputLayout.NumElements = numElements;
}

void GraphicsPSO::SetVertexShader(const void* binary, size_t size)
{
	pipelineStateDesc_.VS = { binary,size };
}

void GraphicsPSO::SetPixelShader(const void* binary, size_t size)
{
	pipelineStateDesc_.PS = { binary,size };
}

void GraphicsPSO::SetBlendState(const D3D12_BLEND_DESC& blendDesc)
{
	pipelineStateDesc_.BlendState = blendDesc;
}

void GraphicsPSO::SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc)
{
	pipelineStateDesc_.RasterizerState = rasterizerDesc;
}

void GraphicsPSO::SetRenderTargetFormats(UINT numRTVs, const DXGI_FORMAT* rtvFormats, DXGI_FORMAT dsvFormat)
{
	pipelineStateDesc_.NumRenderTargets = numRTVs;
	pipelineStateDesc_.DSVFormat = dsvFormat;
	pipelineStateDesc_.SampleDesc.Count = 1;
	pipelineStateDesc_.SampleDesc.Quality = 0;
	for (uint32_t i = 0; i < pipelineStateDesc_.NumRenderTargets; ++i)
	{
		pipelineStateDesc_.RTVFormats[i] = rtvFormats[i];
	}
}

void GraphicsPSO::SetSampleMask(UINT sampleMask)
{
	pipelineStateDesc_.SampleMask = sampleMask;
}

void GraphicsPSO::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType)
{
	pipelineStateDesc_.PrimitiveTopologyType = primitiveTopologyType;
}

void GraphicsPSO::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc)
{
	pipelineStateDesc_.DepthStencilState = depthStencilDesc;
}

void GraphicsPSO::Finalize()
{
	pipelineStateDesc_.pRootSignature = rootSignature_->GetRootSignature();
	ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();
	HRESULT hr = device->CreateGraphicsPipelineState(&pipelineStateDesc_, IID_PPV_ARGS(&pipelineState_));
	if (FAILED(hr)) { assert(SUCCEEDED(hr)); };
}
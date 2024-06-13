#pragma once
#include "PSO.h"

class GraphicsPSO : public PSO
{
public:
	void SetInputLayout(UINT numElements, const D3D12_INPUT_ELEMENT_DESC* inputElementDescs);

	void SetVertexShader(const void* binary, size_t size);

	void SetPixelShader(const void* binary, size_t size);

	void SetBlendState(const D3D12_BLEND_DESC& blendDesc);

	void SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc);

	void SetRenderTargetFormats(UINT numRTVs, const DXGI_FORMAT* rtvFormats, DXGI_FORMAT dsvFormat);

	void SetSampleMask(UINT sampleMask);

	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType);

	void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc);

	void Finalize();

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc_{};
};


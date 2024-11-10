#include "ComputePSO.h"
#include "GraphicsCore.h"

void ComputePSO::SetComputeShader(const void* binary, size_t size)
{
	pipelineStateDesc_.CS = { binary,size };
}

void ComputePSO::Finalize()
{
	pipelineStateDesc_.pRootSignature = rootSignature_->GetRootSignature();
	ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();
	HRESULT hr = device->CreateComputePipelineState(&pipelineStateDesc_, IID_PPV_ARGS(&pipelineState_));
	if (FAILED(hr)) { assert(SUCCEEDED(hr)); };
}
#pragma once
#include "RootSignature.h"

class PSO
{
public:
	~PSO() = default;

	void SetRootSignature(const RootSignature* rootSignature);

	ID3D12PipelineState* GetPipelineState() const { return pipelineState_.Get(); };

protected:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

	const RootSignature* rootSignature_ = nullptr;
};


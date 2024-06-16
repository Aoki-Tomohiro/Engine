#pragma once
#include "PSO.h"

class ComputePSO : public PSO
{
public:
	void SetComputeShader(const void* binary, size_t size);

	void Finalize();

private:
	D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateDesc_{};
};


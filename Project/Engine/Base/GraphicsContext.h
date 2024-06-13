#pragma once
#include "CommandContext.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "RootSignature.h"

class GraphicsContext : public CommandContext
{
public:
	void TransitionResource(GpuResource& resource, D3D12_RESOURCE_STATES newState);

	void SetRenderTargets(UINT num, const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[]);

	void SetRenderTargets(UINT num, const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[], D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);

	void ClearColor(ColorBuffer& target);

	void ClearDepth(DepthBuffer& target);

	void SetViewport(const D3D12_VIEWPORT& viewport);

	void SetScissor(const D3D12_RECT& rect);

	void SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);

	void SetVertexBuffers(UINT startSlot, UINT count, const D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[]);

	void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& indexBufferView);

	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology);

	void SetConstantBuffer(UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS cbv);

	void SetDescriptorTable(UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

	void SetRootSignature(const RootSignature& rootSignature);

	void DrawInstanced(UINT vertexCount, UINT instanceCount);

	void DrawIndexedInstanced(UINT indexCount, UINT instanceCount);
};


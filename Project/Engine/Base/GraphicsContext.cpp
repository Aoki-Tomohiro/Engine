#include "GraphicsContext.h"
#include <cassert>

void GraphicsContext::TransitionResource(GpuResource& resource, D3D12_RESOURCE_STATES newState)
{
	D3D12_RESOURCE_STATES oldState = resource.currentState_;

	if (oldState != newState)
	{
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = resource.GetResource();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = oldState;
		barrier.Transition.StateAfter = newState;
		resource.currentState_ = newState;
		commandList_->ResourceBarrier(1, &barrier);
	}
}

void GraphicsContext::SetRenderTargets(UINT num, const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[])
{
	commandList_->OMSetRenderTargets(num, rtvHandles, false, nullptr);
}

void GraphicsContext::SetRenderTargets(UINT num, const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[], D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
{
	commandList_->OMSetRenderTargets(num, rtvHandles, false, &dsvHandle);
}

void GraphicsContext::ClearColor(ColorBuffer& target)
{
	commandList_->ClearRenderTargetView(target.GetRTVHandle(), target.GetClearColor(), 0, nullptr);
}

void GraphicsContext::ClearDepth(DepthBuffer& target)
{
	commandList_->ClearDepthStencilView(target.GetDSVHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void GraphicsContext::SetViewport(const D3D12_VIEWPORT& viewport)
{
	commandList_->RSSetViewports(1, &viewport);
}

void GraphicsContext::SetScissor(const D3D12_RECT& rect)
{
	commandList_->RSSetScissorRects(1, &rect);
}

void GraphicsContext::SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView)
{
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView);
}

void GraphicsContext::SetVertexBuffers(UINT startSlot, UINT count, const D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[])
{
	commandList_->IASetVertexBuffers(startSlot, count, vertexBufferViews);
}

void GraphicsContext::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& indexBufferView)
{
	commandList_->IASetIndexBuffer(&indexBufferView);
}

void GraphicsContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology)
{
	commandList_->IASetPrimitiveTopology(primitiveTopology);
}

void GraphicsContext::SetConstantBuffer(UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS cbv)
{
	commandList_->SetGraphicsRootConstantBufferView(rootParameterIndex, cbv);
}

void GraphicsContext::SetDescriptorTable(UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
{
	commandList_->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle);
}

void GraphicsContext::SetRootSignature(const RootSignature& rootSignature)
{
	if (rootSignature.GetRootSignature() == currentRootSignature_)
	{
		return;
	}
	commandList_->SetGraphicsRootSignature(currentRootSignature_ = rootSignature.GetRootSignature());
}

void GraphicsContext::DrawInstanced(UINT vertexCount, UINT instanceCount)
{
	commandList_->DrawInstanced(vertexCount, instanceCount, 0, 0);
}

void GraphicsContext::DrawIndexedInstanced(UINT indexCount, UINT instanceCount)
{
	commandList_->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
}
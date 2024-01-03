#pragma once
#include "CommandContext.h"
#include "CommandQueue.h"
#include "Display.h"
#include "DescriptorAllocator.h"
#include "FrameRateController.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class GraphicsCore 
{
public:
	static GraphicsCore* GetInstance();

	static void Destroy();

	void Initialize();

	void PreDraw();

	void PostDraw();

	void ClearRenderTarget();

	void ClearDepthBuffer();

	D3D12_CPU_DESCRIPTOR_HANDLE AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);

	ID3D12Device* GetDevice() const { return device_.Get(); };

	CommandContext* GetCommandContext() const { return commandContext_.get(); };

	CommandQueue* GetCommandQueue() const { return commandQueue_.get(); };

private:
	GraphicsCore() = default;
	~GraphicsCore() = default;
	GraphicsCore(const GraphicsCore&) = delete;
	GraphicsCore& operator=(const GraphicsCore&) = delete;

private:
	static GraphicsCore* instance_;

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

	std::unique_ptr<CommandContext> commandContext_ = nullptr;

	std::unique_ptr<CommandQueue> commandQueue_ = nullptr;

	std::unique_ptr<Display> display_ = nullptr;

	std::unique_ptr<DepthBuffer> depthBuffer_ = nullptr;

	std::unique_ptr<FrameRateController> frameRateController_ = nullptr;

	DescriptorAllocator descriptorAllocators_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES]
	{
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV ,
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
	};
};


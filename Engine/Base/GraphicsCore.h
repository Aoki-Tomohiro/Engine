#pragma once
#include "Application.h"
#include "DescriptorHeap.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include <array>
#include <chrono>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <memory>
#include <thread>
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

	void TransitionResource(GpuResource& resource, D3D12_RESOURCE_STATES newState);

	void ClearRenderTarget();

	void ClearDepthBuffer();

	DescriptorHandle AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);

	ID3D12DescriptorHeap* GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) const { return descriptorHeaps_[type]->GetDescriptorHeap(); }

	ID3D12Device* GetDevice() const { return device_.Get(); };

	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); };

private:
	GraphicsCore() = default;
	~GraphicsCore() = default;
	GraphicsCore(const GraphicsCore&) = delete;
	GraphicsCore& operator=(const GraphicsCore&) = delete;

	void CreateDevice();

	void CreateCommand();

	void CreateSwapChain();

	void CreateFence();

	void CreateDescriptorHeaps();

	void CreateResources();

	void InitializeFixFPS();

	void UpdateFixFPS();

private:
	static GraphicsCore* instance_;

	Application* app_ = nullptr;

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;

	uint64_t fenceValue_{};

	std::array<std::unique_ptr<DescriptorHeap>, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> descriptorHeaps_{};

	std::array<const uint32_t, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> kNumDescriptors_ = { 256, 256, 256, 256, };

	std::array<std::unique_ptr<ColorBuffer>, 2> swapChainResources_;

	std::unique_ptr<DepthBuffer> depthBuffer_ = nullptr;

	std::chrono::steady_clock::time_point reference_{};
};


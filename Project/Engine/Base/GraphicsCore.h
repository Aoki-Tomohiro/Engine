/**
 * @file GraphicsCore.h
 * @brief グラフィックスのコアを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Application.h"
#include "CommandContext.h"
#include "CommandQueue.h"
#include "Display.h"
#include "DescriptorHeap.h"
#include "FrameRateController.h"
#include <array>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <memory>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class GraphicsCore
{
public:
	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static GraphicsCore* GetInstance();

	/// <summary>
	/// 破棄処理
	/// </summary>
	static void Destroy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// レンダーターゲットをクリア
	/// </summary>
	void ClearRenderTarget();

	/// <summary>
	/// 深度バッファをクリア
	/// </summary>
	void ClearDepthBuffer();

	/// <summary>
	/// デスクリプタを割り当てる
	/// </summary>
	/// <param name="type">デスクリプタヒープの種類</param>
	/// <returns>デスクリプタハンドル</returns>
	DescriptorHandle AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);

	/// <summary>
	/// デスクリプタヒープを取得
	/// </summary>
	/// <param name="type">デスクリプタの種類</param>
	/// <returns>デスクリプタヒープ</returns>
	ID3D12DescriptorHeap* GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) const { return descriptorHeaps_[type]->GetDescriptorHeap(); }

	//デバイスを取得
	ID3D12Device* GetDevice() const { return device_.Get(); };

	//コマンドコンテキストを取得
	CommandContext* GetCommandContext() const { return commandContext_.get(); };

	//コマンドキューを取得
	CommandQueue* GetCommandQueue() const { return commandQueue_.get(); };

private:
	GraphicsCore() = default;
	~GraphicsCore() = default;
	GraphicsCore(const GraphicsCore&) = delete;
	GraphicsCore& operator=(const GraphicsCore&) = delete;

private:
	static GraphicsCore* instance_;

	Application* app_ = nullptr;

	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;

	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

	std::unique_ptr<CommandContext> commandContext_ = nullptr;

	std::unique_ptr<CommandQueue> commandQueue_ = nullptr;

	std::array<std::unique_ptr<DescriptorHeap>, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> descriptorHeaps_{};

	const std::array<uint32_t, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> kNumDescriptors_ = { 256, 256, 256, 256, };

	std::unique_ptr<Display> display_ = nullptr;

	std::unique_ptr<DepthBuffer> depthBuffer_ = nullptr;

	std::unique_ptr<FrameRateController> frameRateController_ = nullptr;
};


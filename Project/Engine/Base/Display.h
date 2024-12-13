/**
 * @file Display.h
 * @brief スワップチェーンを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Application.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include <array>
#include <memory>
#include <dxgi1_6.h>

class Display
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxgiFactory">DXGIファクトリ</param>
	/// <param name="commandQueue">コマンドキュー</param>
	void Initialize(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue);

	/// <summary>
	/// 画面の切り替え
	/// </summary>
	void Present();

	//現在のカラーバッファを取得
	ColorBuffer& GetCurrentBuffer();

private:
	/// <summary>
	/// スワップチェーンを作成
	/// </summary>
	/// <param name="dxgiFactory">DXGIファクトリ</param>
	/// <param name="commandQueue">コマンドキュー</param>
	void CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue);

	/// <summary>
	/// リソースの作成
	/// </summary>
	void CreateResources();

private:
	Application* application_ = nullptr;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;

	std::array<std::unique_ptr<ColorBuffer>, 2> swapChainResources_;
};


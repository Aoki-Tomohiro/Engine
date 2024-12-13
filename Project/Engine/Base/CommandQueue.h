/**
 * @file CommandQueue.h
 * @brief コマンドキューを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include <cstdint>
#include <d3d12.h>
#include <wrl.h>

class CommandQueue
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// コマンドリストを実行
	/// </summary>
	/// <param name="commandList">コマンドリストの配列</param>
	void ExecuteCommandList(ID3D12CommandList* commandList[]);

	/// <summary>
	/// フェンス待ち
	/// </summary>
	void WaitForFence();

	//コマンドキューを取得
	ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_.Get(); };

private:
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;

	uint64_t fenceValue_{};
};


/**
 * @file ComputePSO.h
 * @brief コンピュートシェーダー用のPSOを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "PSO.h"

class ComputePSO : public PSO
{
public:
	/// <summary>
	/// コンピュートシェーダーを設定
	/// </summary>
	/// <param name="binary">バイナリデータ</param>
	/// <param name="size">サイズ</param>
	void SetComputeShader(const void* binary, size_t size);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

private:
	D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateDesc_{};
};


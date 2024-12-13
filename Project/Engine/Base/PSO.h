/**
 * @file PSO.h
 * @brief PSOの基底クラスを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "RootSignature.h"

class PSO
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~PSO() = default;

	/// <summary>
	/// ルートシグネチャを設定
	/// </summary>
	/// <param name="rootSignature">ルートシグネチャ</param>
	void SetRootSignature(const RootSignature* rootSignature);

	//パイプラインステートを取得
	ID3D12PipelineState* GetPipelineState() const { return pipelineState_.Get(); };

protected:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;

	const RootSignature* rootSignature_ = nullptr;
};


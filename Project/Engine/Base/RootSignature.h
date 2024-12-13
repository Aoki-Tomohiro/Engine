/**
 * @file RootSignature.h
 * @brief ルートシグネチャを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "RootParameter.h"
#include <cassert>
#include <wrl.h>
#include <memory>

class RootSignature
{
public:
	/// <summary>
	/// ルートシグネチャを生成
	/// </summary>
	/// <param name="numParameters">ルートパラメーターの数</param>
	/// <param name="numSamplers">サンプラーの数</param>
	void Create(UINT numParameters, UINT numSamplers);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// サンプラーの初期化
	/// </summary>
	/// <param name="registerNum">レジスタ番号</param>
	/// <param name="nonStaticSamplerDesc">サンプラーの設定</param>
	/// <param name="shaderVIsibility">どのシェーダーで使うか</param>
	void InitStaticSampler(UINT registerNum, const D3D12_STATIC_SAMPLER_DESC& nonStaticSamplerDesc, D3D12_SHADER_VISIBILITY shaderVIsibility);

	//ルートパラメーターを取得
	RootParameter& operator[] (size_t entryIndex)
	{
		assert(entryIndex < numRootParameters_);
		return rootParameters_.get()[entryIndex];
	}

	//ルートパラメーターを取得（const）
	const RootParameter& operator[](size_t entryIndex) const
	{
		assert(entryIndex < numStaticSamplers_);
		return rootParameters_.get()[entryIndex];
	}

	//ルートシグネチャを取得
	ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); };

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

	std::unique_ptr<RootParameter[]> rootParameters_ = nullptr;

	std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> staticSamplers_ = nullptr;

	UINT numRootParameters_ = 0;

	UINT numStaticSamplers_ = 0;

	UINT numInitializedStaticSamplers_ = 0;
};


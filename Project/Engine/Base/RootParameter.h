/**
 * @file RootParameter.h
 * @brief ルートパラメーターを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include <d3d12.h>

class RootParameter
{
	friend class RootSignature;

public:
	/// <summary>
	/// コンスタントバッファを設定
	/// </summary>
	/// <param name="registerNum">レジスタ番号</param>
	/// <param name="shaderVisibility">どのシェーダーで使うか</param>
	void InitAsConstantBuffer(UINT registerNum, D3D12_SHADER_VISIBILITY shaderVisibility);

	/// <summary>
	/// デスクリプタレンジを設定
	/// </summary>
	/// <param name="type">デスクリプタレンジの種類</param>
	/// <param name="registerNum">レジスタ番号</param>
	/// <param name="count">個数</param>
	/// <param name="shaderVisibility">どのシェーダーで使うか</param>
	void InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE type, UINT registerNum, UINT count, D3D12_SHADER_VISIBILITY shaderVisibility);

	/// <summary>
	/// デスクリプタテーブルを設定
	/// </summary>
	/// <param name="rangeCount">デスクリプタレンジの種類</param>
	/// <param name="shaderVisibility">どのシェーダーで使うか</param>
	void InitAsDescriptorTable(UINT rangeCount, D3D12_SHADER_VISIBILITY shaderVisibility);

	/// <summary>
	/// 目標のデスクリプタレンジを設定
	/// </summary>
	/// <param name="rangeIndex">デスクリプタレンジのインデックス</param>
	/// <param name="type">デスクリプタレンジの種類</param>
	/// <param name="registerNum">レジスタ番号</param>
	/// <param name="count">個数</param>
	void SetTableRange(UINT rangeIndex, D3D12_DESCRIPTOR_RANGE_TYPE type, UINT registerNum, UINT count);

	//ルートパラメーターを取得
	const D3D12_ROOT_PARAMETER& operator()() const { return rootParameter_; };

private:
	D3D12_ROOT_PARAMETER rootParameter_{};
};


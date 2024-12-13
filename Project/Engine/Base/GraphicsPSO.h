/**
 * @file GraphicsPSO.h
 * @brief パイプラインステートを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "PSO.h"

class GraphicsPSO : public PSO
{
public:
	/// <summary>
	/// インプットレイアウトを設定
	/// </summary>
	/// <param name="numElements">属性の数</param>
	/// <param name="inputElementDescs">属性のポインタ</param>
	void SetInputLayout(UINT numElements, const D3D12_INPUT_ELEMENT_DESC* inputElementDescs);

	/// <summary>
	/// 頂点シェーダーを設定
	/// </summary>
	/// <param name="binary">バイナリデータ</param>
	/// <param name="size">サイズ</param>
	void SetVertexShader(const void* binary, size_t size);

	/// <summary>
	/// ピクセルシェーダーを設定
	/// </summary>
	/// <param name="binary">バイナリデータ</param>
	/// <param name="size">サイズ</param>
	void SetPixelShader(const void* binary, size_t size);

	/// <summary>
	/// ブレンドステートを設定
	/// </summary>
	/// <param name="blendDesc">ブレンドの設定</param>
	void SetBlendState(const D3D12_BLEND_DESC& blendDesc);

	/// <summary>
	/// ラスタライザステートを設定
	/// </summary>
	/// <param name="rasterizerDesc">ラスタライザの設定</param>
	void SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc);

	/// <summary>
	/// レンダーターゲットのフォーマットを設定
	/// </summary>
	/// <param name="numRTVs">レンダーターゲットビューの数</param>
	/// <param name="rtvFormats">レンダーターゲットのフォーマット</param>
	/// <param name="dsvFormat">深度バッファビューのフォーマット</param>
	void SetRenderTargetFormats(UINT numRTVs, const DXGI_FORMAT* rtvFormats, DXGI_FORMAT dsvFormat);

	/// <summary>
	/// サンプラーを設定
	/// </summary>
	/// <param name="sampleMask">サンプラーのマスク</param>
	void SetSampleMask(UINT sampleMask);

	/// <summary>
	/// 形状を設定
	/// </summary>
	/// <param name="primitiveTopologyType">形状</param>
	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE primitiveTopologyType);

	/// <summary>
	/// デプスステンシルステートを設定
	/// </summary>
	/// <param name="depthStencilDesc">デプスステンシルの設定</param>
	void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc_{};
};


/**
 * @file CommandContext.h
 * @brief コマンドを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "RootSignature.h"
#include "PSO.h"

class CommandContext
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// リソースの状態を変更
	/// </summary>
	/// <param name="resource">変更するリソース</param>
	/// <param name="newState">新しい状態</param>
	void TransitionResource(GpuResource& resource, D3D12_RESOURCE_STATES newState);

	/// <summary>
	/// UAVバリアを挿入
	/// </summary>
	/// <param name="resource">UAVバリアを挿入するリソース</param>
	void InsertUAVBarrier(GpuResource& resource);

	/// <summary>
	/// レンダーターゲットを設定
	/// </summary>
	/// <param name="num">個数</param>
	/// <param name="rtvHandles">Rtvハンドル</param>
	void SetRenderTargets(UINT num, const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[]);

	/// <summary>
	/// レンダーターゲットを設定
	/// </summary>
	/// <param name="num">個数</param>
	/// <param name="rtvHandles">Rtvハンドル</param>
	/// <param name="dsvHandle">Dsvハンドル</param>
	void SetRenderTargets(UINT num, const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[], D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);

	/// <summary>
	/// レンダーターゲットをクリア
	/// </summary>
	/// <param name="target">クリアするカラーバッファ</param>
	void ClearColor(ColorBuffer& target);

	/// <summary>
	/// 深度バッファをクリア
	/// </summary>
	/// <param name="target">クリアする深度バッファ</param>
	void ClearDepth(DepthBuffer& target);

	/// <summary>
	/// ビューポートを設定
	/// </summary>
	/// <param name="viewport">ビューポート</param>
	void SetViewport(const D3D12_VIEWPORT& viewport);

	/// <summary>
	/// シザー矩形を設定
	/// </summary>
	/// <param name="rect">シザー矩形</param>
	void SetScissor(const D3D12_RECT& rect);

	/// <summary>
	/// デスクリプタヒープを設定
	/// </summary>
	/// <param name="type">デスクリプタヒープの種類</param>
	/// <param name="descriptorHeap">デスクリプタヒープ</param>
	void SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, ID3D12DescriptorHeap* descriptorHeap);

	/// <summary>
	/// 頂点バッファを設定
	/// </summary>
	/// <param name="vertexBufferView">頂点バッファビュー</param>
	void SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);

	/// <summary>
	/// 複数の頂点バッファを設定
	/// </summary>
	/// <param name="startSlot">開始のスロット</param>
	/// <param name="count">個数</param>
	/// <param name="vertexBufferViews">頂点バッファビューの配列</param>
	void SetVertexBuffers(UINT startSlot, UINT count, const D3D12_VERTEX_BUFFER_VIEW vertexBufferViews[]);

	/// <summary>
	/// インデックスバッファを設定
	/// </summary>
	/// <param name="indexBufferView">インデックスバッファビュー</param>
	void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW& indexBufferView);

	/// <summary>
	/// 形状を設定
	/// </summary>
	/// <param name="primitiveTopology">形状</param>
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology);

	/// <summary>
	/// コンスタントバッファを設定
	/// </summary>
	/// <param name="rootParameterIndex">ルートパラメーターの番号</param>
	/// <param name="cbv">コンスタントバッファビュー</param>
	void SetConstantBuffer(UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS cbv);

	/// <summary>
	/// デスクリプタテーブルを設定
	/// </summary>
	/// <param name="rootParameterIndex">ルートパラメーターの番号</param>
	/// <param name="gpuHandle">GPUハンドル</param>
	void SetDescriptorTable(UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

	/// <summary>
	/// ルートシグネチャを設定
	/// </summary>
	/// <param name="rootSignature">ルートシグネチャ</param>
	void SetRootSignature(const RootSignature& rootSignature);

	/// <summary>
	/// パイプラインステートを設定
	/// </summary>
	/// <param name="pipelineState">パイプラインステート</param>
	void SetPipelineState(const PSO& pipelineState);

	/// <summary>
	/// 描画命令を飛ばす
	/// </summary>
	/// <param name="vertexCount">頂点数</param>
	/// <param name="instanceCount">インスタンス数</param>
	void DrawInstanced(UINT vertexCount, UINT instanceCount);

	/// <summary>
	/// インデックス描画命令を飛ばす
	/// </summary>
	/// <param name="indexCount">インデックスの数</param>
	/// <param name="instanceCount">インスタンス数</param>
	void DrawIndexedInstanced(UINT indexCount, UINT instanceCount);

	/// <summary>
	/// コンピュートシェーダー用のルートシグネチャを設定
	/// </summary>
	/// <param name="rootSignature">コンピュートシェーダー用のルートシグネチャ</param>
	void SetComputeRootSignature(const RootSignature& rootSignature);

	/// <summary>
	/// コンピュートシェーダー用のデスクリプタテーブルを設定
	/// </summary>
	/// <param name="rootParameterIndex">ルートパラメーターの番号</param>
	/// <param name="gpuHandle">GPUハンドル</param>
	void SetComputeDescriptorTable(UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

	/// <summary>
	/// コンピュートシェーダー用のコンスタントバッファを設定
	/// </summary>
	/// <param name="rootParameterIndex">ルートパラメーターの番号</param>
	/// <param name="cbv">コンスタントバッファビュー</param>
	void SetComputeConstantBuffer(UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS cbv);

	/// <summary>
	/// 指定されたスレッドグループ数でコンピュートシェーダを実行
	/// </summary>
	/// <param name="groupCountX">スレッドグループのX軸方向の数</param>
	/// <param name="groupCountY">スレッドグループのY軸方向の数</param>
	/// <param name="groupCountZ">スレッドグループのZ軸方向の数</param>
	void Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ);

	/// <summary>
	/// コマンドを閉じる
	/// </summary>
	void Close();

	/// <summary>
	/// コマンドリセット
	/// </summary>
	void Reset();

	//コマンドリストを取得
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); };

private:
	/// <summary>
	/// デスクリプタヒープをバインド
	/// </summary>
	void BindDescriptorHeaps();

protected:
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	ID3D12RootSignature* currentRootSignature_ = nullptr;

	ID3D12PipelineState* currentPipelineState_ = nullptr;

	ID3D12DescriptorHeap* currentDescriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
};


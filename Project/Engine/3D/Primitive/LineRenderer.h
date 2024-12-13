/**
 * @file LineRenderer.h
 * @brief ライン描画を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/3D/Camera/Camera.h"
#include <vector>

class LineRenderer
{
public:
	//ラインの最大数
	static const uint32_t kMaxLines = 2048;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static LineRenderer* GetInstance();

	/// <summary>
	/// 破棄処理
	/// </summary>
	static void Destroy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// ラインを追加
	/// </summary>
	/// <param name="start">開始座標</param>
	/// <param name="end">終了座標</param>
	void AddLine(const Vector3& start, const Vector3& end);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//カメラを取得・設定
	const Camera* GetCamera() const { return camera_; };
	void SetCamera(const Camera* camera) { camera_ = camera; };

private:
	LineRenderer() = default;
	~LineRenderer() = default;
	LineRenderer(const LineRenderer&) = delete;
	LineRenderer& operator=(const LineRenderer&) = delete;

private:
	static LineRenderer* instance_;

	RootSignature rootSignature_{};

	GraphicsPSO pipelineState_{};

	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::vector<Vector4> vertices_{};

	const Camera* camera_ = nullptr;
};


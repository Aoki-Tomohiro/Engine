/**
 * @file TrailRenderer.h
 * @brief 軌跡を生成や描画を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/3D/Camera/Camera.h"
#include "Trail.h"
#include <vector>

class TrailRenderer
{
public:
	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static TrailRenderer* GetInstance();

	/// <summary>
	/// 破棄処理
	/// </summary>
	static void Destroy();

	/// <summary>
	/// 軌跡を生成
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>軌跡</returns>
	static Trail* CreateTrail(const std::string& name);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//カメラを取得・設定
	const Camera* GetCamera() const { return camera_; };
	void SetCamera(const Camera* camera) { camera_ = camera; };

private:
	TrailRenderer() = default;
	~TrailRenderer() = default;
	TrailRenderer(const TrailRenderer&) = delete;
	TrailRenderer& operator=(const TrailRenderer&) = delete;

	/// <summary>
	/// 規制を内部で生成
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>軌跡</returns>
	Trail* CreateTrailInternal(const std::string& name);

private:
	static TrailRenderer* instance_;

	RootSignature rootSignature_{};

	GraphicsPSO pipelineState_{};

	std::unordered_map<std::string, std::unique_ptr<Trail>> trails_;

	const Camera* camera_ = nullptr;
};


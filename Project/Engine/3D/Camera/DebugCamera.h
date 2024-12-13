/**
 * @file DebugCamera.h
 * @brief デバッグカメラを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/Components/Input/Input.h"
#include "Camera.h"

class DebugCamera 
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	//カメラを取得
	const Camera& GetCamera() { return camera_; };

	//追従対象を設定
	void SetTarget(const WorldTransform* target) { target_ = target; };

private:
	//入力クラス
	Input* input_ = nullptr;

	//ワールドトランスフォーム
	WorldTransform worldTransform_{};

	//カメラ
	Camera camera_{};

	//オフセット
	Vector3 offset_ = { 0.0f,0.0f,-50.0f };

	//累積回転行列
	Matrix4x4 matRot_{};

	//追従対象
	const WorldTransform* target_ = nullptr;
};


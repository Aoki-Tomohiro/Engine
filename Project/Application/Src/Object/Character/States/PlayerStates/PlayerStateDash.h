#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Math/MathFunction.h"

/// <summary>
/// ダッシュ状態
/// </summary>
class PlayerStateDash : public IPlayerState
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="other">衝突相手</param>
	void OnCollision(GameObject* other) override;

private:
	/// <summary>
	/// 速度移動イベントを実行
	/// </summary>
	/// <param name="velocityMovementEvent">速度移動イベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param> 
	void InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex) override;

	/// <summary>
	/// ダッシュアニメーション終了の確認と遷移
	/// </summary>
	void CheckTransitionToDashEndAnimation();

	/// <summary>
	/// ダッシュ終了処理
	/// </summary>
	void FinalizeDash();

	/// <summary>
	/// 速度移動イベントの処理
	/// </summary>
	void HandleVelocityMovement();

	/// <summary>
	/// 煙エミッターの位置を更新
	/// </summary>
	void UpdateSmokeEmitters();

private:
	//オーディオ
	Audio* audio_ = nullptr;

	//現在のアニメーションの名前
	std::string currentAnimation_ = "DashStart";

	//ダッシュのオーディオハンドル
	uint32_t dashAudioHandle_ = 0;

	//煙のパーティクルのオフセット値
	Vector3 dashParticleOffset_ = { 0.0f,0.0f,-2.0f };
};


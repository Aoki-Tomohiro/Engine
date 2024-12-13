/**
 * @file CharacterStateDeath.h
 * @brief キャラクターの死亡状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "ICharacterState.h"

class CharacterStateDeath : public ICharacterState
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
	/// <param name="">衝突相手</param>
	void OnCollision(GameObject*) override {};

private:
	/// <summary>
	/// イージング移動イベントの初期化
	/// </summary>
	/// <param name="">イージング移動イベント</param>
	/// <param name="">アニメーションイベントのインデックス</param> 
	void InitializeVelocityMovement(const VelocityMovementEvent*, const int32_t) override {};

	/// <summary>
	/// イージング移動イベントの初期化
	/// </summary>
	/// <param name="">イージング移動イベント</param>
	/// <param name="">アニメーションイベントのインデックス</param>
	void InitializeEasingMovementEvent(const EasingMovementEvent*, const int32_t) override {};

	/// <summary>
	/// デフォルトの状態遷移処理
	/// </summary>
	void HandleStateTransitionInternal() override {};

	/// <summary>
	/// 地面に埋まらないように座標を補正
	/// </summary>
	void ClampPositionToGround();
};


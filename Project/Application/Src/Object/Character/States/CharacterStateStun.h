/**
 * @file CharacterStateFactory.h
 * @brief キャラクターのスタン状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "AbstractCharacterState.h"

class CharacterStateStun : public AbstractCharacterState
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~CharacterStateStun() override = default;

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
	/// 速度移動イベントの初期化
	/// </summary>
	/// <param name="">速度移動イベント</param>
	/// <param name="">アニメーションイベントのインデックス</param> 
	void InitializeVelocityMovement(const VelocityMovementEvent*, const int32_t) override {};

	/// <summary>
	/// イージング移動イベントの初期化
	/// </summary>
	/// <param name="">イージング移動イベント</param>
	/// <param name="">アニメーションイベントのインデックス</param> 
	void InitializeEasingMovementEvent(const EasingMovementEvent*, const int32_t) override {};

	/// <summary>
	/// リアクションのアニメーションを再生
	/// </summary>
	/// <param name="reactionType">リアクションタイプ</param>
	void PlayReactionAnimation(const ReactionType reactionType);

	/// <summary>
	/// 地面に埋まらないように座標を補正
	/// </summary>
	void AdjustPositionToGround();

	/// <summary>
	/// 必要に応じてアニメーションを一時停止
	/// </summary>
	void PauseAnimationIfRequired();

	/// <summary>
	/// スタン状態からの回復を管理
	/// </summary>
	void HandleStunRecovery();

	/// <summary>
	/// 立ち上がりアニメーションと状態遷移を管理
	/// </summary>
	void ManageStandUpAnimationAndTransition();

private:
	//現在のリアクションのタイプ
	ReactionType currentReactionType_ = ReactionType::kFront;

	//スタン状態が現在アクティブかどうか
	bool isStunActive_ = true;

	//立ち上がりアニメーションが再生されているか
	bool isPlayStundupAnimation_ = false;

	//ノックバックアニメーションを停止する時間
	float knockbackAnimationPauseTime_ = 0.8f;
};


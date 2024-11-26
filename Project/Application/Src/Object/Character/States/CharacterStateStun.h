#pragma once
#include "ICharacterState.h"

/// <summary>
/// スタン状態
/// </summary>
class CharacterStateStun : public ICharacterState
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
	void ClampPositionToGround();

	/// <summary>
	/// リアクションタイプに基づいて状態を遷移
	/// </summary>
	void HandleStateTransitionBasedOnReaction();

private:
	//現在のリアクションのタイプ
	ReactionType currentReactionType_ = ReactionType::kFlinch;

	//スタン状態が現在アクティブかどうか
	bool isStunActive_ = true;
};


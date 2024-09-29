#pragma once
#include "Application/Src/Object/Enemy/States/IEnemyState.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

class EnemyStateStun : public IEnemyState
{
public:
	void Initialize() override;

	void Update() override;

	void OnCollision(GameObject* other) override;

private:
	void PlayReactionAnimation(ReactionType reactionType, float speed);

	void AdjustEnemyPosition();

	void RecoverFromStun();

	void ApplyDamageAndKnockback(const float damage, const KnockbackSettings& knockbackSettings, const bool applyKnockback);

	void ApplyKnockbackAndPlayStunAnimation(const KnockbackSettings& knockbackSettings);

private:
	//現在のリアクションのタイプ
	ReactionType currentReactionType_ = ReactionType::Flinch;

	//スタンが終了したかどうか
	bool isStunFinished_ = false;
};


#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"

class PlayerStateJustDodgeAttack : public IPlayerState
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(GameObject* other);

	void OnCollisionEnter(GameObject* other);

	void OnCollisionExit(GameObject* other);

private:
	//回避攻撃用構造体
	struct WorkJustDodgeAttack
	{
		Vector3 startPosition;	         // 始点座標
		Vector3 targetPosition;          // 終点座標
		bool isMovementFinished = false; // 移動が終了したかどうか
		float moveTimer = 0.0f;          // 回避攻撃の移動用タイマー
		float hitTimer = 0.0f;           // ヒット用タイマー
		int32_t hitCount = 0;            // ヒット回数
	};

private:
	//回避攻撃用ワーク
	WorkJustDodgeAttack justDodgeAttackWork_{};
};


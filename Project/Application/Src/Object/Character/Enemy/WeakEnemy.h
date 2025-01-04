/**
 * @file WeakEnemy.h
 * @brief 弱い敵を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Application/Src/Object/Character/BaseCharacter.h"

class WeakEnemy : public BaseCharacter
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
	/// <param name="gameObject">衝突相手</param>
	void OnCollision(GameObject* gameObject) override;

	/// <summary>
	/// プレイヤーの方向に敵を向かせる処理
	/// </summary>
	void LookAtPlayer();

private:
};


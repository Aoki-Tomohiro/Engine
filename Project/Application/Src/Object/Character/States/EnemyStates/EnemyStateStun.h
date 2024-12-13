/**
 * @file EnemyStateStun.h
 * @brief 敵のスタン状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Application/Src/Object/Character/States/CharacterStateStun.h"

class EnemyStateStun : public CharacterStateStun
{
private:
	/// <summary>
	/// デフォルトの状態遷移処理
	/// </summary>
	void HandleStateTransitionInternal() override;
};


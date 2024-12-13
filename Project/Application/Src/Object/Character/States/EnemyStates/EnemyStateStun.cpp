/**
 * @file EnemyStateStun.cpp
 * @brief 敵のスタン状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#include "EnemyStateStun.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void EnemyStateStun::HandleStateTransitionInternal()
{
	//遷移可能なアクション一覧
	const std::vector<std::string> actions = { "Idle", "MoveTowardPlayer", "MoveAwayFromPlayer", "MoveSideToPlayer" };

	//アクションリストをループし、遷移条件をチェック
	for (const auto& action : actions)
	{
		//遷移条件が満たされていた場合
		if (character_->IsActionExecutable(action))
		{
			//対応する状態に遷移
			character_->ChangeState(action);
			return;
		}
	}
}
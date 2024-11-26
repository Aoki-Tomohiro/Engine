#pragma once
#include "Application/Src/Object/Character/States/CharacterStateStun.h"

/// <summary>
/// スタン状態
/// </summary>
class PlayerStateStun : public CharacterStateStun
{
private:
	/// <summary>
	/// デフォルトの状態遷移処理
	/// </summary>
	void HandleStateTransitionInternal() override;
};


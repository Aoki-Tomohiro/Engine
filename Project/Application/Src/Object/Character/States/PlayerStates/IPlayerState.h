#pragma once
#include "Engine/Components/Input/Input.h"
#include "Application/Src/Object/Character/States/ICharacterState.h"

class Player;

class IPlayerState : public ICharacterState
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~IPlayerState() = default;

	/// <summary>
	/// プレイヤーを設定
	/// </summary>
	/// <param name="player">プレイヤー</param>
	void SetPlayer(Player* player) { player_ = player; };

protected:
	//プレイヤー
	Player* player_ = nullptr;
};


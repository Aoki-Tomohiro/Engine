#pragma once
#include "Application/Src/Object/Character/States/ICharacterState.h"
#include "Engine/Components/Input/Input.h"

class Player;

class IPlayerState : public ICharacterState
{
public:
	virtual ~IPlayerState() override = default;

	//プレイヤーを取得
	Player* GetPlayer() const;

protected:
	//インプット
	Input* input_ = nullptr;
};
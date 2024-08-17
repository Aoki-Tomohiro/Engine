#pragma once

class Player;

class IPlayerState
{
public:
	virtual void Initialize() = 0;

	virtual void Update() = 0;

	void SetPlayer(Player* player) { player_ = player; };

protected:
	Player* player_ = nullptr;
};
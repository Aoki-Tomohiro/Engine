#pragma once

class Player;
class GameObject;

class IPlayerState
{
public:
	virtual ~IPlayerState() = default;

	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void OnCollision(GameObject* other) = 0;

	void SetPlayer(Player* player) { player_ = player; };

protected:
	Player* player_ = nullptr;
};
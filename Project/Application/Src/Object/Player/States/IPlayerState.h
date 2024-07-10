#pragma once
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Framework/Object/GameObject.h"
#include <string>

class Player;

class IPlayerState
{
public:
	virtual ~IPlayerState() = default;

	virtual void Initialize() = 0;

	virtual void Update() = 0;

	virtual void Draw(const Camera& camera) = 0;

	virtual void OnCollision(GameObject* other) = 0;

	virtual void OnCollisionEnter(GameObject* other) = 0;

	virtual void OnCollisionExit(GameObject* other) = 0;

	void SetPlayer(Player* player) { player_ = player; };

protected:
	Player* player_ = nullptr;
};


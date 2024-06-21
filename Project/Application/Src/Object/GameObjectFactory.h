#pragma once
#include "Engine/Framework/Object/AbstractGameObjectFactory.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"

class GameObjectFactory : public AbstractGameObjectFactory
{
public:
	GameObject* CreateGameObject(const std::string& objectName) override;
};


#pragma once
#include "Engine/Framework/Object/AbstractGameObjectFactory.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Ground/Ground.h"
#include "Application/Src/Object/Warning/Warning.h"

class GameObjectFactory : public AbstractGameObjectFactory
{
public:
	GameObject* CreateGameObject(const std::string& objectName) override;
};


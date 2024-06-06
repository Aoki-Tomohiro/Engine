#pragma once
#include "Engine/Framework/Object/AbstractGameObjectFactory.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Boss/Boss.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/Weapon.h"
#include "Application/Src/Object/Ground/Ground.h"

class GameObjectFactory : public AbstractGameObjectFactory
{
public:
	IGameObject* CreateGameObject(const std::string& objectName) override;
};


#pragma once
#include "Engine/Framework/Object/AbstractGameObjectFactory.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Ground/Ground.h"
#include "Application/Src/Object/Skydome/Skydome.h"
#include "Application/Src/Object/BackGroundObject/BackGroundObject.h"

class GameObjectFactory : public AbstractGameObjectFactory
{
public:
	GameObject* CreateGameObject(const std::string& objectName) override;
};


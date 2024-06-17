#pragma once
#include "Engine/Framework/Object/AbstractGameObjectFactory.h"

class GameObjectFactory : public AbstractGameObjectFactory
{
public:
	IGameObject* CreateGameObject(const std::string& objectName) override;
};


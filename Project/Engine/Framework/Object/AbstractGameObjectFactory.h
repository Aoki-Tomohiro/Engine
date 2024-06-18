#pragma once
#include "GameObject.h"
#include <string>

class AbstractGameObjectFactory
{
public:
	virtual ~AbstractGameObjectFactory() = default;

	virtual GameObject* CreateGameObject(const std::string& objectName) = 0;
};
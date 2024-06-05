#pragma once
#include "IGameObject.h"
#include <string>

class AbstractGameObjectFactory
{
public:
	virtual ~AbstractGameObjectFactory() = default;

	virtual IGameObject* CreateGameObject(const std::string& objectName) = 0;
};
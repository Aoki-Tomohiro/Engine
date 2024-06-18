#include "GameObjectFactory.h"

GameObject* GameObjectFactory::CreateGameObject(const std::string& objectName)
{
	if (objectName == "Player")
	{
		Player* player = new Player();
		return player;
	}
	else if (objectName == "Enemy")
	{
		Enemy* enemy = new Enemy();
		return enemy;
	}

	return nullptr;
}
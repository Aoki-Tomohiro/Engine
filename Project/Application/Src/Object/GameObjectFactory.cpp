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
	else if (objectName == "Ground")
	{
		Ground* ground = new Ground();
		return ground;
	}
	else if (objectName == "Weapon")
	{
		Weapon* weapon = new Weapon();
		return weapon;
	}
	else if (objectName.find("BackGroundObject") != std::string::npos)
	{
		BackGroundObject* backGroundObject = new BackGroundObject();
		return backGroundObject;
	}

	return nullptr;
}
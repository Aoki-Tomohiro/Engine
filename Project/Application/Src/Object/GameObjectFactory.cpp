#include "GameObjectFactory.h"

GameObject* GameObjectFactory::CreateGameObject(const std::string& objectName)
{
	if (objectName == "Player")
	{
		Player* player = new Player();
		return player;
	}
	else if (objectName == "Weapon")
	{
		Weapon* weapon = new Weapon();
		return weapon;
	}
	else if (objectName == "Enemy")
	{
		Enemy* enemy = new Enemy();
		return enemy;
	}
	else if (objectName == "Ground")
	{
		Ground* ground = new Ground;
		return ground;
	}
	else if (objectName == "Warning")
	{
		Warning* warning = new Warning;
		return warning;
	}
	else if (objectName == "MagicProjectile")
	{
		MagicProjectile* magicProjectile = new MagicProjectile;
		return magicProjectile;
	}
	else if (objectName.find("BackGround") != std::string::npos)
	{
 		BackgroundObject* backGroundObject = new BackgroundObject();
		return backGroundObject;
	}

	return nullptr;
}
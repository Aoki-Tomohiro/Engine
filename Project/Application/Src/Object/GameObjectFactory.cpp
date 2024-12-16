/**
 * @file GameObjectFactory.cpp
 * @brief ゲームオブジェクトを生成するファクトリー
 * @author 青木智滉
 * @date
 */

#include "GameObjectFactory.h"

GameObject* GameObjectFactory::CreateGameObject(const std::string& objectName)
{
	if (objectName.find("GameObject") != std::string::npos)
	{
		GameObject* gameObject = new GameObject();
		return gameObject;
	}
	else if (objectName == "Player")
	{
		Player* player = new Player();
		return player;
	}
	else if (objectName == "Enemy")
	{
		Enemy* enemy = new Enemy();
		return enemy;
	}
	else if (objectName == "Weapon")
	{
		Weapon* weapon = new Weapon();
		return weapon;
	}
	else if (objectName == "Ground")
	{
		Ground* ground = new Ground();
		return ground;
	}
	else if (objectName == "Skydome")
	{
		Skydome* skydome = new Skydome();
		return skydome;
	}
	else if (objectName.find("BackGroundObject") != std::string::npos)
	{
		BackGroundObject* backGroundObject = new BackGroundObject();
		return backGroundObject;
	}

	return nullptr;
}
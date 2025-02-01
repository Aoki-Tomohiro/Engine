/**
 * @file GameObjectFactory.h
 * @brief ゲームオブジェクトを生成するファクトリー
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Framework/Object/AbstractGameObjectFactory.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Ground/Ground.h"
#include "Application/Src/Object/Skydome/Skydome.h"
#include "Application/Src/Object/BackGroundObject/BackGroundObject.h"
#include "Application/Src/Object/BreakableObject/BreakableObject.h"

class GameObjectFactory : public AbstractGameObjectFactory
{
public:
	/// <summary>
	/// ゲームオブジェクトを生成
	/// </summary>
	/// <param name="objectName">オブジェクトの名前</param>
	/// <returns>ゲームオブジェクト</returns>
	GameObject* CreateGameObject(const std::string& objectName) override;
};


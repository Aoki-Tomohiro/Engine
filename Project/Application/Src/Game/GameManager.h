/**
 * @file GameManager.h
 * @brief ゲーム全体を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Framework/Game/GameCore.h"
#include "Application/Src/Scene/SceneFactory.h"
#include "Application/Src/Object/GameObjectFactory.h"

class GameManager : public GameCore
{
public:
	void Initialize() override;
};


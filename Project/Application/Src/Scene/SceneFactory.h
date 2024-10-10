#pragma once
#include "Engine/Framework/Scene/AbstractSceneFactory.h"
#include "GameTitleScene.h"
#include "GamePlayScene.h"
#include "SampleScene.h"
#include "LoadScene.h"

class SceneFactory : public AbstractSceneFactory
{
public:
	IScene* CreateScene(const std::string& sceneName) override;
};


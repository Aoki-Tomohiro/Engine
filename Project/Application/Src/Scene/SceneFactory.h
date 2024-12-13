/**
 * @file SceneFactory.h
 * @brief シーンを生成するファクトリー
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Framework/Scene/AbstractSceneFactory.h"
#include "GameTitleScene.h"
#include "GamePlayScene.h"
#include "SampleScene.h"
#include "LoadScene.h"

class SceneFactory : public AbstractSceneFactory
{
public:
	/// <summary>
	/// シーンを生成
	/// </summary>
	/// <param name="sceneName">シーンの名前</param>
	/// <returns>シーン</returns>
	IScene* CreateScene(const std::string& sceneName) override;
};


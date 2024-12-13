/**
 * @file SceneFactory.cpp
 * @brief シーンを生成するファクトリー
 * @author 青木智滉
 * @date
 */

#include "SceneFactory.h"

IScene* SceneFactory::CreateScene(const std::string& sceneName) 
{
	//次のシーンを生成
	IScene* newScene = nullptr;

	if (sceneName == "GameTitleScene") 
	{
		newScene = new GameTitleScene();
	}
	else if (sceneName == "GamePlayScene")
	{
		newScene = new GamePlayScene();
	}
	else if (sceneName == "SampleScene")
	{
		newScene = new SampleScene();
	}
	else if (sceneName == "LoadScene")
	{
		newScene = new LoadScene();
	}

	return newScene;
}
/**
 * @file AbstractSceneFactory.h
 * @brief シーンを生成するファクトリーの仮想クラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "IScene.h"
#include <string>

class AbstractSceneFactory
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~AbstractSceneFactory() = default;

	/// <summary>
	/// シーンを生成
	/// </summary>
	/// <param name="sceneName">シーンの名前</param>
	/// <returns>生成したシーン</returns>
	virtual IScene* CreateScene(const std::string& sceneName) = 0;
};
/**
 * @file AbstractGameObjectFactory.h
 * @brief ゲームオブジェクトのファクトリー
 * @author 青木智滉
 * @date
 */

#pragma once
#include "GameObject.h"
#include <string>

class AbstractGameObjectFactory
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~AbstractGameObjectFactory() = default;

	/// <summary>
	/// ゲームオブジェクトを生成
	/// </summary>
	/// <param name="objectName">ゲームオブジェクトの名前</param>
	/// <returns>生成したゲームオブジェクト</returns>
	virtual GameObject* CreateGameObject(const std::string& objectName) = 0;
};
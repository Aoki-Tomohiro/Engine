/**
 * @file Ground.h
 * @brief 地面を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Model/ModelComponent.h"

class Ground : public GameObject
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private:
	//モデル
	ModelComponent* model_ = nullptr;

	//色
	Vector4 color_ = { 0.118f,0.118f,0.118f,1.0f };

	//鏡面反射の色
	Vector3 specularColor_ = { 0.0f,0.0f,0.0f };

	//UVスケール
	Vector2 uvScale_ = { 600.0f,600.0f };
};


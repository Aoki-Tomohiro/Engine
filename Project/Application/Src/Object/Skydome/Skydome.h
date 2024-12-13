/**
 * @file Skydome.h
 * @brief 天球を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Model/ModelComponent.h"

class Skydome : public GameObject
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

	//影を描画するかどうか
	bool castShadows_ = false;

	//ライティングをするかどうか
	bool isEnableLighting_ = false;

	//環境の映り込みを調整するための係数
	float environmentCoefficient_ = 0.0f;

	//UVScale
	Vector2 uvScale_ = { 10.0f,10.0f };
};


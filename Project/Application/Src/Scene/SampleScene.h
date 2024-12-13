/**
 * @file SampleScene.h
 * @brief サンプルシーンを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/3D/Camera/CameraManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/LevelManager/LevelManager.h"

class SampleScene : public IScene
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// UIの描画
	/// </summary>
	void DrawUI() override;

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	Camera* camera_ = nullptr;

	GameObjectManager* gameObjectManager_ = nullptr;
};


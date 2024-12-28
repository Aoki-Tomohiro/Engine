/**
 * @file GameTitleScene.h
 * @brief ゲームタイトルシーンを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/LevelManager/LevelManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"
#include "Application/Src/Object/Transition/Transition.h"
#include "Application/Src/Object/UI/UIManager.h"
#include "Application/Src/Object/LevelSelector/LevelSelector.h"
#include <numbers>

class GameTitleScene : public IScene
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override {};

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
	/// <summary>
	/// カメラの更新
	/// </summary>
	void UpdateCamera();

	/// <summary>
	/// 難易度選択メニューの処理
	/// </summary>
	void HandleDifficultyMenu();

	/// <summary>
	/// タイトルメニューの処理
	/// </summary>
	void HandleTitleMenu();

	/// <summary>
	/// フェードを更新してシーンを変える
	/// </summary>
	void HandleFadeAndSceneTransition();

	/// <summary>
	/// 決定ボタンが押されたかどうかを取得
	/// </summary>
	/// <returns>決定ボタンが押されたかどうか</returns>
	bool IsActionButtonPressed();

	/// <summary>
	/// キャンセルボタンが押されたかどうかを取得
	/// </summary>
	/// <returns>キャンセルボタンが押されたかどうか</returns>
	bool IsCancelButtonPressed();

private:
	//レンダラー
	Renderer* renderer_ = nullptr;

	//インプット
	Input* input_ = nullptr;

	//オーディオ
	Audio* audio_ = nullptr;

	//カメラ
	Camera* camera_ = nullptr;

	//ゲームオブジェクトマネージャー
	GameObjectManager* gameObjectManager_ = nullptr;

	//パーティクルマネージャー
	ParticleManager* particleManager_ = nullptr;

	//敵
	Enemy* enemy_ = nullptr;

	//トランジション
	std::unique_ptr<Transition> transition_ = nullptr;

	//UIマネージャー
	std::unique_ptr<UIManager> uiManager_ = nullptr;

	//レベルセレクト
	std::unique_ptr<LevelSelector> levelSelector_ = nullptr;

	//ゲームタイトルのUI
	UIElement* titleUI_{};

	//オーディオハンドル
	uint32_t audioHandle_ = 0;

	//ボイスハンドル
	uint32_t voiceHandle_ = 0;
};


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
	/// <summary>
	/// カメラの更新
	/// </summary>
	void UpdateCamera();

	/// <summary>
	/// 難易度選択UIの更新
	/// </summary>
	void UpdateDifficultyUI();

	/// <summary>
	/// フェードを更新してシーンを変える
	/// </summary>
	void HandleFadeAndSceneTransition();

	/// <summary>
	/// ボタンが押されたかどうかを取得
	/// </summary>
	/// <returns>ボタンが押されたかどうか</returns>
	bool IsActionButtonPressed();

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

	//タイトルのスプライト
	std::unique_ptr<Sprite> titleSprite_ = nullptr;
	std::unique_ptr<Sprite> pressASprite_ = nullptr;

	//難易度メニューのスプライト
	std::unique_ptr<Sprite> difficultyMenuSprite_ = nullptr;
	Vector4 difficultyMenuSpriteColor_ = { 0.0f, 0.0f, 0.0f, 0.6f };

	//難易度メニューのテキストのスプライト
	std::unique_ptr<Sprite> difficultyMenuTextSprite_ = nullptr;
	Vector2 difficultyMenuTextPosition_ = { 640.0f, 160.0f };

	//難易度のスプライト
	std::array<std::unique_ptr<Sprite>, Enemy::Level::Count> levelSprites_{};

	//難易度のスプライトの座標
	std::array<Vector2, Enemy::Level::Count> levelSpritePosition_{ {
		{640.0f, 280.0f}, {640.0f, 380.0f}, {640.0f, 480.0f},}
	};

	//オーディオハンドル
	uint32_t audioHandle_ = 0;

	//ボイスハンドル
	uint32_t voiceHandle_ = 0;

	//敵のレベル
	Enemy::Level difficultyLevel_ = Enemy::Level::Easy;

	//難易度調整の項目が表示されているか
	bool isDifficultyMenuVisible_ = false;

	//難易度変更のタイマー
	float difficultyChangeTimer_ = 0.0f;
};


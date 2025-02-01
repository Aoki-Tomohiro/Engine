/**
 * @file GamePlayScene.h
 * @brief ゲームプレイシーンを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/3D/Primitive/TrailRenderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/Collision/CollisionManager.h"
#include "Engine/LevelManager/LevelManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Magic/Magic.h"
#include "Application/Src/Object/BreakableObject/BreakableObject.h"
#include "Application/Src/Object/HitStop/HitStop.h"
#include "Application/Src/Object/Lockon/Lockon.h"
#include "Application/Src/Object/Transition/Transition.h"
#include "Application/Src/Object/Editors/EditorManager.h"
#include "Application/Src/Object/UI/UIManager.h"

class GamePlayScene : public IScene
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
	/// カメラコントローラーの初期化
	/// </summary>
	void InitializeCameraController();

	/// <summary>
	/// プレイヤーの初期化
	/// </summary>
	void InitializePlayer();

	/// <summary>
	/// 敵の初期化
	/// </summary>
	void InitializeEnemy();

	/// <summary>
	/// キャラクターの初期化
	/// </summary>
	/// <param name="character">初期化するキャラクター</param>
	void InitializeCharacter(BaseCharacter* character);

	/// <summary>
	/// 武器の初期化
	/// </summary>
	/// <param name="character">持たせるキャラクター</param>
	void InitializeWeapon(BaseCharacter* character);

	/// <summary>
	/// スプライトの初期化
	/// </summary>
	void InitializeSprites();

	/// <summary>
	/// BGMの読み込みと再生
	/// </summary>
	void LoadAndPlayBGM();

	/// <summary>
	/// コライダーの更新
	/// </summary>
	void UpdateColliders();

	/// <summary>
	/// カメラとロックオンの更新
	/// </summary>
	void UpdateCameraAndLockOn();

	/// <summary>
	/// トランジションの処理
	/// </summary>
	void HandleTransition();

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

	//衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;

	//コライダーの配列
	std::vector<Collider*> colliders_{};

	//プレイヤー
	Player* player_ = nullptr;

	//敵
	Enemy* enemy_ = nullptr;

	//カメラコントローラー
	std::unique_ptr<CameraController> cameraController_ = nullptr;

	//ヒットストップ
	std::unique_ptr<HitStop> hitStop_ = nullptr;

	//ロックオン
	std::unique_ptr<Lockon> lockon_ = nullptr;

	//トランジション
	std::unique_ptr<Transition> transition_ = nullptr;

	//エディターマネージャー
	std::unique_ptr<EditorManager> editorManager_ = nullptr;

	//UIマネージャー
	std::unique_ptr<UIManager> uiManager_ = nullptr;

	//ゲームオーバーのUI
	UIElement* gameOverUI_ = nullptr;

	//ゲームクリアのUI
	UIElement* gameClearUI_ = nullptr;

	//オーディオハンドル
	uint32_t audioHandle_ = 0;

	//ボイスハンドル
	uint32_t voiceHandle_ = 0;
};


#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/Model/AnimationManager.h"
#include "Engine/3D/Primitive/TrailRenderer.h"
#include "Engine/3D/Skybox/Skybox.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/Collision/CollisionManager.h"
#include "Engine/LevelLoader/LevelLoader.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Laser/Laser.h"
#include "Application/Src/Object/Pillar/Pillar.h"
#include "Application/Src/Object/MagicProjectile/MagicProjectile.h"
#include "Application/Src/Object/combatAnimationEditor/combatAnimationEditor.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/CameraPathManager.h"
#include "Application/Src/Object/Lockon/Lockon.h"
#include "Application/Src/Object/HitStop/HitStop.h"
#include "Application/Src/Object/Transition/Transition.h"

class GamePlayScene : public IScene
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

private:
	void InitializeManagers();

	void InitializeCameraAndLockon();

	void InitializePlayer();

	void InitializePlayerWeapon();

	void InitializeEnemy();

	void InitializeEnemyWeapon();

	void InitializeCameraController();

	void UpdateColliders();

	void HandleCameraShake();

	void UpdateCameraAndLockOn();

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

	//プレイヤー
	Player* player_ = nullptr;

	//プレイヤーの武器
	Weapon* playerWeapon_ = nullptr;

	//敵
	Enemy* enemy_ = nullptr;

	//敵の武器
	Weapon* enemyWeapon_ = nullptr;

	//衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;

	//CombatAnimationEditor
	std::unique_ptr<CombatAnimationEditor> combatAnimationEditor_ = nullptr;

	//カメラコントローラー
	std::unique_ptr<CameraController> cameraController_ = nullptr;

	//カメラパスマネージャー
	std::unique_ptr<CameraPathManager> cameraPathManager_ = nullptr;

	//ロックオン
	std::unique_ptr<Lockon> lockon_ = nullptr;

	//ヒットストップ
	std::unique_ptr<HitStop> hitStop_ = nullptr;

	//トランジション
	std::unique_ptr<Transition> transition_ = nullptr;

	//スカイボックス
	std::unique_ptr<Skybox> skybox_ = nullptr;

	//ゲームオーバーのスプライト
	std::unique_ptr<Sprite> gameOverSprite_ = nullptr;
	bool isGameOver_ = false;

	//ゲームクリアのスプライト
	std::unique_ptr<Sprite> gameClearSprite_ = nullptr;
	bool isGameClear_ = false;

	//オーディオハンドル
	uint32_t audioHandle_ = 0;
};


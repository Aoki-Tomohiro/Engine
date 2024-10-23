#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/3D/Primitive/TrailRenderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/Collision/CollisionManager.h"
#include "Engine/LevelManager/LevelManager.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Laser/Laser.h"
#include "Application/Src/Object/Pillar/Pillar.h"
#include "Application/Src/Object/Magic/Magic.h"
#include "Application/Src/Object/HitStop/HitStop.h"
#include "Application/Src/Object/Lockon/Lockon.h"
#include "Application/Src/Object/Transition/Transition.h"
#include "Application/Src/Object/Editors/EditorManager.h"

class GamePlayScene : public IScene
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

private:
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

	//衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;

	////プレイヤー
	//Player* player_ = nullptr;

	////敵
	//Enemy* enemy_ = nullptr;

	////カメラコントローラー
	//std::unique_ptr<CameraController> cameraController_ = nullptr;

	//ヒットストップ
	std::unique_ptr<HitStop> hitStop_ = nullptr;

	//ロックオン
	std::unique_ptr<Lockon> lockon_ = nullptr;

	//トランジション
	std::unique_ptr<Transition> transition_ = nullptr;

	//エディターマネージャー
	std::unique_ptr<EditorManager> editorManager_ = nullptr;

	//ゲームオーバーのスプライト
	std::unique_ptr<Sprite> gameOverSprite_ = nullptr;
	bool isGameOver_ = false;

	//ゲームクリアのスプライト
	std::unique_ptr<Sprite> gameClearSprite_ = nullptr;
	bool isGameClear_ = false;

	//オーディオハンドル
	uint32_t audioHandle_ = 0;
};


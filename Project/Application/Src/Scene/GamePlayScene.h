#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/3D/Model/AnimationManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/Collision/CollisionManager.h"
#include "Engine/LevelLoader/LevelLoader.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Camera/CameraPathManager.h"
#include "Application/Src/Object/Lockon/Lockon.h"
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
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	Camera* camera_ = nullptr;

	//ゲームオブジェクトマネージャー
	GameObjectManager* gameObjectManager_ = nullptr;

	//パーティクルマネージャー
	ParticleManager* particleManager_ = nullptr;

	//衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;

	//カメラコントローラー
	std::unique_ptr<CameraController> cameraController_ = nullptr;

	//カメラパスマネージャー
	std::unique_ptr<CameraPathManager> cameraPathManager_ = nullptr;

	//ロックオン
	std::unique_ptr<Lockon> lockon_ = nullptr;

	//トランジション
	std::unique_ptr<Transition> transition_ = nullptr;
};


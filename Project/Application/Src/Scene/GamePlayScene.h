#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/3D/Camera/CameraManager.h"
#include "Engine/3D/Skybox/Skybox.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Collision/CollisionManager.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/3D/Camera/LockOn.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/CameraController/CameraController.h"
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
	//HitStop用構造体
	struct HitStopSettings 
	{
		bool isActive = false;// ヒットストップがアクティブかどうか
		float duration = 0.08f;// ヒットストップの持続時間（秒）
		float timer = 0.0f;// ヒットストップのタイマー（秒）
	};

	//Parry用の構造体
	struct ParrySettings
	{
		bool isActive = false;// パリィがアクティブかどうか
		bool isSlow = false;// パリィのスローモーションがアクティブかどうか
		float duration = 10.0f;// パリィの持続時間（秒）
		float stopDuration = 1.0f;// パリィ後の時間停止の持続時間（秒）
		float timer = 0.0f;// パリィのタイマー（秒）
	};

	void HandleTransition();

	void UpdateHitStop();

	void UpdateParry();

	void ApplyGlobalVariables();

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	//Camera
	Camera* camera_ = nullptr;

	//GameObjectManager
	GameObjectManager* gameObjectManager_ = nullptr;

	//ParticleManager
	ParticleManager* particleManager_ = nullptr;

	//CameraController
	std::unique_ptr<CameraController> cameraController_ = nullptr;

	//LockOn
	std::unique_ptr<LockOn> lockOn_ = nullptr;

	//Transition
	std::unique_ptr<Transition> transition_ = nullptr;

	//CollisionManager
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;

	//Skybox
	std::unique_ptr<Skybox> skybox_ = nullptr;

	//ヒットストップの変数
	HitStopSettings hitStopSettings_{};

	//パリィの変数
	ParrySettings parrySettings_{};

	//ゲームオーバーのスプライト
	std::unique_ptr<Sprite> gameOverSprite_ = nullptr;
	bool isGameOver_ = false;

	//ゲームクリアのスプライト
	std::unique_ptr<Sprite> gameClearSprite_ = nullptr;
	bool isGameClear_ = false;
};


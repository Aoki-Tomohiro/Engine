#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/Collision/CollisionManager.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/LevelLoader/LevelLoader.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Camera/CameraPathManager.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Lockon/Lockon.h"
#include "Application/Src/Object/AnimationStateManager/AnimationStateManager.h"

class GamePlayScene : public IScene
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

private:
	//ヒットストップ用構造体
	struct HitStopSettings
	{
		bool isActive = false;  // ヒットストップがアクティブかどうか
		float duration = 0.12f; // ヒットストップの持続時間
		float timer = 0.0f;     // ヒットストップのタイマー
	};

	//パリィ用の構造体
	struct ParrySettings
	{
		bool isActive = false;             // パリィがアクティブかどうか
		bool isSlow = false;               // パリィのスローモーションがアクティブかどうか
		float duration = 8.0f;             // パリィの持続時間
		float stopDuration = 1.0f;         // パリィ後の時間停止の持続時間
		float timer = 0.0f;                // パリィのタイマー
		float graySaturation = -0.4f;      // 最終的のグレーの色
		float grayDuration = 0.1f;         // グレーになるまでの時間
		float grayTimer = 0.0f;            // グレーになるまでの計測用タイマー
	};

	void UpdateHitStop();

	void UpdateParry();

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	Camera* camera_ = nullptr;

	GameObjectManager* gameObjectManager_ = nullptr;

	ParticleManager* particleManager_ = nullptr;

	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;

	std::unique_ptr<CameraPathManager> cameraPathManager_ = nullptr;

	std::unique_ptr<CameraController> cameraController_ = nullptr;

	std::unique_ptr<Lockon> lockon_ = nullptr;

	std::unique_ptr<AnimationStateManager> animationStateManager_ = nullptr;

	//ヒットストップの変数
	HitStopSettings hitStopSettings_{};

	//パリィの変数
	ParrySettings parrySettings_{};
};


#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/3D/Skybox/Skybox.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Collision/CollisionManager.h"
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
	void HandleTransition();

	void UpdateHitStop();

	void UpdateCameraShake();

	void ApplyGlobalVariables();

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	//Camera
	Camera camera_{};

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
	bool isHitStopActive_ = false;
	float hitStopDuration_ = 0.06f;
	float hitStopTimer_ = 0.0f;

	//カメラシェイクの変数
	bool isShaking_ = false;
	Vector3 shakeIntensity_{ 0.0f,2.0f,0.0f };
	Vector3 shakeoffset_{ 0.0f,0.0f,0.0f };
	float shakeDuration_ = 0.1f;
	float shakeTimer_ = 0.0f;

	//次のシーン
	enum NextScene
	{
		kGameClearScene,
		kGameOverScene
	};
	NextScene nextScene_ = kGameClearScene;
};


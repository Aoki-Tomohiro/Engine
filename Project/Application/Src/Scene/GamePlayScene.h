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
	void HandleTransition();

	void UpdateHitStop();

	void UpdateParry();

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

	//パリィの変数
	bool isParrySlowMotionActive_ = false;
	float parryDuration_ = 10.0f;
	float parryTimer_ = 0.0f;

	//DoF
	float focusDepth_ = 0.0f;
	float nFocusWidth_ = 0.005f;
	float fFocusWidth_ = 0.01f;

	//次のシーン
	enum NextScene
	{
		kGameClearScene,
		kGameOverScene
	};
	NextScene nextScene_ = kGameClearScene;

	//ゲームオーバーのスプライト
	std::unique_ptr<Sprite> gameOverSprite_ = nullptr;
	bool isGameOver_ = false;

	//ゲームクリアのスプライト
	std::unique_ptr<Sprite> gameClearSprite_ = nullptr;
	bool isGameClear_ = false;
};


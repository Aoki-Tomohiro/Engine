#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Collision/CollisionManager.h"
#include "Engine/3D/Camera/FollowCamera.h"
#include "Engine/3D/Camera/LockOn.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Weapon/Weapon.h"
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

private:
	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	//GameObjectManager
	GameObjectManager* gameObjectManager_ = nullptr;

	//ParticleManager
	ParticleManager* particleManager_ = nullptr;

	//Camera
	Camera* camera_ = nullptr;

	//FollowCamera
	std::unique_ptr<FollowCamera> followCamera_ = nullptr;

	//LockOn
	std::unique_ptr<LockOn> lockOn_ = nullptr;

	//Transition
	std::unique_ptr<Transition> transition_ = nullptr;

	//CollisionManager
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;

	//次のシーン
	enum NextScene
	{
		kGameClearScene,
		kGameOverScene
	};
	NextScene nextScene_ = kGameClearScene;
};


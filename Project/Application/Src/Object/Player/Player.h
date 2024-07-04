#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/3D/Camera/LockOn.h"
#include "Engine/Math/MathFunction.h"
#include "States/IPlayerState.h"

class Player : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void SetCamera(const Camera* camera) { camera_ = camera; };

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

private:
	void ChangeState(IPlayerState* state);

private:
	//プレイヤーの状態
	std::unique_ptr<IPlayerState> state_ = nullptr;

	//速度
	Vector3 velocity{};

	//Quaternion
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	//Camera
	const Camera* camera_ = nullptr;

	//LockOn
	const LockOn* lockOn_ = nullptr;

	//タイトルシーンなのか
	bool isInTitleScene_ = false;

	//Collider
	Vector3 colliderOffset_{ 0.0f,2.2f,0.0f };

	//フレンドクラスに登録
	friend class PlayerStateIdle;
	friend class PlayerStateJump;
	friend class PlayerStateDodge;
	friend class PlayerStateDash;
	friend class PlayerStateGroundAttack;

#pragma region アニメーションの名前一覧
	//Armature.001|mixamo.com|Layer0
	//Armature.001|mixamo.com|Layer0.001
	//Armature.001|mixamo.com|Layer0.002
	//Armature.001|mixamo.com|Layer0.003
	//Armature.001|mixamo.com|Layer0.004
	//Armature.001|mixamo.com|Layer0.005
	//Armature.001|mixamo.com|Layer0.006
	//Armature.001|mixamo.com|Layer0.007
	//Armature|mixamo.com|Layer0
#pragma endregion
};


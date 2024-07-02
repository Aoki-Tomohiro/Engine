#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "States/IPlayerState.h"
#include "Engine/Math/MathFunction.h"

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

	//タイトルシーンなのか
	bool isInTitleScene_ = false;

	//Collider関連
	Vector3 colliderOffset_{};
	Vector3 min_ = { -0.6f,-1.8f,-0.6f };
	Vector3 max_ = { 0.6f,1.8f,0.8f };

	//フレンドクラスに登録
	friend class PlayerStateIdle;
	friend class PlayerStateJump;
	friend class PlayerStateDodge;
	friend class PlayerStateDash;
	friend class PlayerStateGroundAttack;
};


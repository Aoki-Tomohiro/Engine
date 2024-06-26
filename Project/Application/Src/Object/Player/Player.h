#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "States/PlayerStateIdle.h"

class Player : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* collider) override;

	void OnCollisionEnter(GameObject* collider) override;

	void OnCollisionExit(GameObject* collider) override;

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

	//フレンドクラスに登録
	friend class PlayerStateIdle;
	friend class PlayerStateJump;
	friend class PlayerStateDodge;
	friend class PlayerStateDash;
};


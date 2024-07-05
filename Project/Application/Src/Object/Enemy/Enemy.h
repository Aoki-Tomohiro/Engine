#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"
#include "States/EnemyStateIdle.h"

class Enemy : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

	const bool GetIsAttack() const;

private:
	void ChangeState(IEnemyState* state);

private:
	//プレイヤーの状態
	std::unique_ptr<IEnemyState> state_ = nullptr;

	//速度
	Vector3 velocity{};

	float parryTimeSpeed_ = 1.0f;

	//Quaternion
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	//タイトルシーンなのか
	bool isInTitleScene_ = false;

	//Collider
	Vector3 colliderOffset_{ 0.0f,4.0f,0.0f };

	//フレンドクラスに登録
	friend class EnemyStateIdle;
	friend class EnemyStateTackle;
};


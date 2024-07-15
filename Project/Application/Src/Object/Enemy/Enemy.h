#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/2D/Sprite.h"
#include "States/EnemyStateIdle.h"

class Enemy : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	const bool GetIsWarning() const { return state_->GetIsWarning(); };

	const bool GetIsAttack() const { return state_->GetIsAttack(); };

	void SetTimeScale(const float timeScale) { timeScale_ = timeScale; };

	void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

private:
	void ChangeState(IEnemyState* state);

private:
	//プレイヤーの状態
	std::unique_ptr<IEnemyState> state_ = nullptr;

	//速度
	Vector3 velocity{};

	//TimeScale
	float timeScale_ = 1.0f;

	//Quaternion
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	//タイトルシーンなのか
	bool isInTitleScene_ = false;

	//Debugモードなのか
	bool isDebug_ = false;

	//アニメーションの時間
	float animationTime_ = 0.0f;

	//フレンドクラスに登録
	friend class EnemyStateIdle;
	friend class EnemyStateTackle;
};


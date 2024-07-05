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

	const bool GetIsAttack() const { return state_->GetIsAttack(); };

	const Vector3& GetColliderOffset() const { return colliderOffset_; };

	const float GetHP() const { return hp_; };

	void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

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

	//HP
	std::unique_ptr<Sprite> spriteHpBar_ = nullptr;
	std::unique_ptr<Sprite> spriteHpBarFrame_ = nullptr;
	Vector2 hpBarSize_{ 480.0f,16.0f };
	const float kMaxHP = 800.0f;
	float hp_ = kMaxHP;

	float disolveParameter_ = 0.0f;

	//フレンドクラスに登録
	friend class EnemyStateIdle;
	friend class EnemyStateTackle;
};


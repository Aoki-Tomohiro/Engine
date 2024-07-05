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

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void SetCamera(const Camera* camera) { camera_ = camera; };

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

	const uint32_t GetComboIndex() const;

	const uint32_t GetComboNum();

	const float GetHP() const { return hp_; };

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

	//HP
	std::unique_ptr<Sprite> spriteHpBar_ = nullptr;
	std::unique_ptr<Sprite> spriteHpBarFrame_ = nullptr;
	Vector2 hpBarSize_{ 480.0f,16.0f };
	const float kMaxHP = 40.0f;
	float hp_ = kMaxHP;

	//ダメージエフェクトのスプライト
	std::unique_ptr<Sprite> damageSprite_ = nullptr;
	Vector4 damageSpriteColor_ = { 1.0f,0.0f,0.0f,0.0f };

	//無敵状態の変数
	bool isInvincible_ = false;
	float invincibleDuration_ = 1.0f;
	float invincibleTimer_ = 0;

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


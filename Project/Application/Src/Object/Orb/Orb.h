#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Lockon/Lockon.h"
#include "Application/Src/Object/MagicProjectile/MagicProjectile.h"

class Orb : public GameObject
{
public:
	//魔法攻撃用ワーク
	struct MagicAttackWork
	{
		bool isCharging = false;    //魔法がチャージされているかどうか
		float chargeTimer = 0.0f;   //チャージ用のタイマー
		float cooldownTimer = 0.0f; //クールタイム用のタイマー
	};

	//魔法攻撃用のパラメーター
	struct MagicAttackParameters
	{
		float chargeTimeThreshold = 1.0f;                                 // チャージマジックのため時間
		float cooldownTime = 0.6f;                                        // 通常魔法のクールタイム
		float magicProjectileSpeed = 96.0f;                               // 魔法の速度
		float normalMagicDamage = 4.0f;                                   // 通常魔法のダメージ
		float chargeMagicDamage = 8.0f;                                   // チャージマジックのダメージ
		Vector3 chargeMagicKnockbackVelocity{0.0f, 30.0f, 12.0f};         // チャージマジックのノックバック速度
		Vector3 chargeMagicKnockbackAcceleration{ 0.0f, -20.0f, -10.0f }; // チャージマジックのノックバック加速度
		float chargeMagicKnockbackDuration = 2.0f;                        // チャージマジックのノックバク持続時間
	};

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void SetLockon(const Lockon* lockOn) { lockon_ = lockOn; };

	void SetTarget(const WorldTransform* target) { target_ = target; };

private:
	void InitializeTransform();

	void InitializeModel();

	void UpdateFollowTarget();

	void UpdatePlayerInfo();

	void UpdateTransform();

	void UpdateUVAnimation();

	void AddMagicProjectile(MagicProjectile::MagicType type);

	Vector3 CalculateMagicProjectileVelocity();

	void InitializeMagicProjectileTransform(MagicProjectile* magicProjectile);

	void InitializeMagicProjectileModel(MagicProjectile* magicProjectile);

	void InitializeMagicProjectileCollider(MagicProjectile* magicProjectile);

	void CreateChargeMagicShotParticles();

private:
	//インプット
	Input* input_ = nullptr;

	//オーディオ
	Audio* audio_ = nullptr;

	//パーティクル
	ParticleSystem* particleSystem_ = nullptr;

	//ロックオン
	const Lockon* lockon_ = nullptr;

	//追従対象
	const WorldTransform* target_ = nullptr;

	//追従対象の残像座標
	Vector3 interTarget_{};

	//追従対象の補間の速度
	float targetInterpolationSpeed_ = 0.2f;

	//追従対象からのオフセット
	Vector3 offset_{ 2.0f, 2.0f, -2.0f };

	//Quaternion
	Quaternion destinationQuaternion_ = Mathf::IdentityQuaternion();

	//Quaternionの補間速度
	float quaternionInterpolationSpeed_ = 0.04f;

	//色
	Vector4 color_ = { 1.0f, 0.265f, 0.0f, 1.0f };

	//UV座標
	Vector2 uvTranslation_{};

	//UVアニメーションの速度
	float uvAnimationSpeed_ = 1.0f;

	//魔法攻撃用ワーク
	MagicAttackWork magicAttackWork_{};

	//魔法攻撃用のパラメーター
	MagicAttackParameters magicAttackParameters_{};

	//通常魔法のオーディオハンドル
	uint32_t fireAudioHandle_ = 0;

	//チャージショットのオーディオハンドル
	uint32_t chargeFireAudioHandle_ = 0;
};


#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

class MagicProjectile : public GameObject
{
public:
	//カウンター
	static int32_t counter;

	//魔法のタイプ
	enum class MagicType
	{
		kNormal,       // 通常の魔法
		kCharged,      // チャージされた魔法
	};

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; };

	const MagicType& GetMagicType() const { return magicType_; };

	void SetMagicType(const MagicType& magicType) { magicType_ = magicType; };

	const KnockbackSettings& GetKnockbackSettings() const { return knockbackSettings_; };

	void SetKnockbackSettings(const KnockbackSettings& knockbackSettings) { knockbackSettings_ = knockbackSettings; };

	const float GetDamage() const { return damage_; };

	void SetDamage(const float damage) { damage_ = damage; };

private:
	void InitializeTransform();

	void InitializeModel();

	void InitializeCollider();

	void InitializeParticleSystem();

	void UpdateMovement();

	void UpdateEmitterPosition();

	void UpdateParticleEmitterVelocity();

	void CheckOutOfBounds();

	void DeleteMagicProjectile();

	void CreateDestoryParticles();

private:
	//パーティクル
	ParticleSystem* particleSystem_ = nullptr;

	//エミッター
	ParticleEmitter* emitter_ = nullptr;

	//インスタンスID
	int32_t id_ = 0;

	//速度
	Vector3 velocity_{};

	//魔法のタイプ
	MagicType magicType_ = MagicType::kNormal;

	//ノックバックの設定
	KnockbackSettings knockbackSettings_{};

	//ダメージ
	float damage_ = 1.0f;
};


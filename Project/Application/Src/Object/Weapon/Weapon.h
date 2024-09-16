#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"
#include "Application/Src/Object/HitStop/HitStop.h"

class Weapon : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void SetHitbox(const Hitbox& hitbox);

	const EffectSettings& GetEffectSettings() const { return effectSettings_; };

	void SetEffectSettings(const EffectSettings& effectSettings) { effectSettings_ = effectSettings; };

	const KnockbackSettings& GetKnockbackSettings() const { return knockbackSettings_; };

	void SetKnockbackSettings(const KnockbackSettings& knockbackSettings) { knockbackSettings_ = knockbackSettings; };

	const float GetDamage() const { return damage_; };

	void SetDamage(const float damage) { damage_ = damage; };

	const bool GetIsAttack() const { return isAttack_; };

	void SetIsAttack(const bool isAttack) { isAttack_ = isAttack; };

	const bool GetIsHit() const { return isHit_; };

	void SetHitStop(HitStop* hitStop) { hitStop_ = hitStop; };

private:
	void UpdateObjectNameFromCollider();

	void CreateAccelerationField();

	void InitializeGlobalVariables();

	void UpdatePlayerCollider();

	void UpdateEnemyCollider();

	void ApplyGlobalVariables();

	void UpdateImGui();

private:
	//オーディオ
	Audio* audio_ = nullptr;

	//パーティクル
	std::map<std::string, ParticleSystem*> particleSystems_{};

	//ヒットストップ
	HitStop* hitStop_ = nullptr;

	//Colliderのオフセット
	Vector3 defaultColliderOffset_{ 0.0f, 0.0f, 3.0f };

	//Colliderのサイズ
	Vector3 defaultColliderSize_{ 2.0f, 2.0f, 2.0f };

	//エフェクトの設定
	EffectSettings effectSettings_{};

	//ノックバックの設定
	KnockbackSettings knockbackSettings_{};

	//ヒットボックス
	Hitbox hitbox_{};

	//ダメージ
	float damage_ = 0.0f;

	//攻撃フラグ
	bool isAttack_ = false;

	//ヒットフラグ
	bool isHit_ = false;

	//デバッグフラグ
	bool isDebug_ = false;

	//オーディオハンドル
	uint32_t hitAudioHandle_ = 0;
};


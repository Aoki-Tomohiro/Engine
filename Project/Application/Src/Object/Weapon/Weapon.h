#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/Primitive/TrailRenderer.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"
#include "Application/Src/Object/HitStop/HitStop.h"

class Weapon : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

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

	void SetIsTrailActive(const bool isTrailActive) { isTrailActive_ = isTrailActive; };

	void SetHitStop(HitStop* hitStop) { hitStop_ = hitStop; };

private:
	void InitializeTransform();

	void InitializeModel();

	void InitializeCollider();

	void InitializeParticleSystem();

	void CreateAccelerationField();

	void InitializeGlobalVariables();

	void InitializeTrail();

	void InitializeHitAudio();

	void UpdateCollider();

	void SetColliderTransform(GameObject* gameObject);

	void UpdateTrail();

	void ApplyGlobalVariables();

	void UpdateImGui();

	void HandlePlayerWeaponCollision();

	void CreateParticleEmitters();

	void CreateShockWaveEmitters(const Vector3& emitterTranslation, Camera* camera);

	void CreateSparkEmitter(const Vector3& emitterTranslation);

	void CreateStarEmitter(const Vector3& position);

	ParticleEmitter* CreateShockWaveEmitter(const Vector3& position, const Vector3& velocity);

private:
	//オーディオ
	Audio* audio_ = nullptr;

	//トランスフォーム
	TransformComponent* transform_ = nullptr;

	//モデル
	ModelComponent* model_ = nullptr;

	//コライダー
	OBBCollider* collider_ = nullptr;

	//パーティクル
	std::map<std::string, ParticleSystem*> particleSystems_{};
	std::unique_ptr<Model> sparkParticleModel_ = nullptr;

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

	//軌跡
	Trail* trail_ = nullptr;

	//軌跡のアクティブフラグ
	bool isTrailActive_ = false;

	//軌跡の色
	Vector4 trailColor_ = { 1.0f, 0.147f, 0.0f, 1.0f };

	//軌跡のオフセット値
	Vector3 headOffset_ = { 0.0f, 0.0f, -30.0f };
	Vector3 frontOffset_ = { 0.0f, 0.0f, -110.0f };
 };


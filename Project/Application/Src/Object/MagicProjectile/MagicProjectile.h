#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Math/MathFunction.h"

class MagicProjectile : public GameObject
{
public:
	//カウンター
	static int32_t counter;

	//魔法のタイプ
	enum class MagicType
	{
		kNormal,       // 通常の魔法
		kEnhanced,     // 強化された魔法
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

	const MagicType GetMagicType() const { return magicType_; };

	void SetMagicType(const MagicType magicType) { magicType_ = magicType; };

	const bool GetIsHit() const { return isHit_; };

private:
	void DeleteMagicProjectile();

private:
	//自分のID
	int32_t id_ = 0;
	
	//速度
	Vector3 velocity_{};

	//魔法のタイプ
	MagicType magicType_ = MagicType::kNormal;

	//ヒットフラグ
	bool isHit_ = false;

	//パーティクル
	ParticleSystem* particleSystem_ = nullptr;
};


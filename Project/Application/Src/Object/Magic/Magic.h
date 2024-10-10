#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Collision/SphereCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"

/// <summary>
/// 魔法
/// </summary>
class Magic : public GameObject
{
public:
	//カウンター
	static int32_t counter;

	//魔法のタイプ
	enum class MagicType
	{
		kNormal,  // 通常の魔法
		kCharged, // チャージされた魔法
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="gameObject">衝突相手</param>
	void OnCollision(GameObject* gameObject);

	//魔法のタイプを設定・取得
	const MagicType& GetMagicType() const { return magicType_; };
	void SetMagicType(const MagicType& magicType) { magicType_ = magicType; };

	//ノックバックの設定を設定・取得
	const KnockbackSettings& GetKnockbackSettings() const { return knockbackSettings_; };
	void SetKnockbackSettings(const KnockbackSettings& knockbackSettings) { knockbackSettings_ = knockbackSettings; };

	//ダメージを設定・取得
	const float GetDamage() const { return damage_; };
	void SetDamage(const float damage) { damage_ = damage; };

	//速度を設定
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; };

private:
	/// <summary>
	/// トランスフォームの初期化
	/// </summary>
	void InitializeTransform();

	/// <summary>
	/// モデルの初期化
	/// </summary>
	void InitializeModel();

	/// <summary>
	/// コライダーの初期化
	/// </summary>
	void InitializeCollider();

	/// <summary>
	/// パーティクルの初期化
	/// </summary>
	void InitializeParticleSystem();

	/// <summary>
	/// 移動パーティクルの生成
	/// </summary>
	void CreateMoveEmitter();

	/// <summary>
	/// 移動のパーティクルの更新
	/// </summary>
	void UpdateMoveEmitter();

	/// <summary>
	/// 画面外に出ているかを確認し出ていた場合は破壊する処理
	/// </summary>
	void CheckOutOfBounds();

	/// <summary>
	/// 魔法の破壊処理
	/// </summary>
	void DeleteMagic();

	/// <summary>
	/// 破壊パーティクルを出す
	/// </summary>
	void CreateDestoryParticles();

private:
	//トランスフォームコンポーネント
	TransformComponent* transformComponent_ = nullptr;

	//モデルコンポーネント
	ModelComponent* modelComponent_ = nullptr;

	//魔法のタイプ
	MagicType magicType_ = MagicType::kNormal;

	//ノックバックの設定
	KnockbackSettings knockbackSettings_{};

	//ダメージ
	float damage_ = 1.0f;

	//大きさ
	Vector3 scale_ = { 0.2f,0.2f,0.2f };

	//速度
	Vector3 velocity_{};

	//パーティクル
	ParticleSystem* particleSystem_ = nullptr;

	//エミッター
	ParticleEmitter* emitter_ = nullptr;

	//パーティクルの速度
	float normalMagicMinSpeed_ = 0.2f;
	float normalMagicMaxSpeed_ = 0.3f;
	float chargeMagicMinSpeed_ = 0.6f;
	float chargeMagicMaxSpeed_ = 0.7f;

	//パーティクルの生成数
	int32_t normalMagicEmitCount_ = 20;
	int32_t chargeMagicEmitCount_ = 40;

	//インスタンスID
	int32_t id_ = 0;
};


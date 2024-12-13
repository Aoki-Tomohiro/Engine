/**
 * @file Magic.h
 * @brief 魔法を管理するファイル
 * @author 青木智滉
 * @date
 */

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
#include "Application/Src/Object/Editors/EditorManager.h"

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
	const KnockbackParameters& GetKnockbackParameters() const { return knockbackParameters_; };
	void SetKnockbackParameters(const KnockbackParameters& knockbackParameters) { knockbackParameters_ = knockbackParameters; };

	//ダメージを設定・取得
	const float GetDamage() const { return damage_; };
	void SetDamage(const float damage) { damage_ = damage; };

	//速度を設定
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; };

	//パーティクルエフェクトエディターを設定
	void SetParticleEffectEditor(ParticleEffectEditor* particleEffectManager) { particleEffectEditor_ = particleEffectManager; };

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

private:
	//トランスフォームコンポーネント
	TransformComponent* transformComponent_ = nullptr;

	//モデルコンポーネント
	ModelComponent* modelComponent_ = nullptr;

	//魔法のタイプ
	MagicType magicType_ = MagicType::kNormal;

	//ノックバックのパラメーター
	KnockbackParameters knockbackParameters_{};

	//ダメージ
	float damage_ = 1.0f;

	//大きさ
	Vector3 scale_ = { 0.2f,0.2f,0.2f };

	//速度
	Vector3 velocity_{};

	//エミッター
	ParticleEmitter* emitter_ = nullptr;

	//インスタンスID
	int32_t id_ = 0;

	//パーティクルエフェクトエディター
	ParticleEffectEditor* particleEffectEditor_ = nullptr;
};


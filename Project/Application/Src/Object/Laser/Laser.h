#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"

/// <summary>
/// レーザー
/// </summary>
class Laser : public GameObject
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	//ダメージを設定・取得
	const float GetDamage() const { return damage_; };
	void SetDamage(const float damage) { damage_ = damage; };

	//座標を設定
	void SetPosition(const Vector3& position) { transform_->worldTransform_.translation_ = position; };

	//回転を設定
	void SetRotation(const Quaternion& quaternion) { transform_->worldTransform_.quaternion_ = quaternion; };

	//目標スケールを設定
	void SetTargetScale(const Vector3& targetScale) { targetScale_ = targetScale; };

	//寿命を設定
	void SetLifeTime(const float lifeTime) { lifeTime_ = lifeTime; };

	//イージングの速度を設定
	void SetEasingSpeed(const float easingSpeed) { easingSpeed_ = easingSpeed; };

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
	/// レーザーのフェード処理
	/// </summary>
	void UpdateLaserFading();

	/// <summary>
	/// レーザーのフェードアウト処理
	/// </summary>
	void FadeOutLaser();

	/// <summary>
	/// コライダーの更新
	/// </summary>
	void UpdateCollider();

private:
	//トランスフォーム
	TransformComponent* transform_ = nullptr;

	//モデル
	ModelComponent* modelComponent_ = nullptr;

	//コライダー
	OBBCollider* collider_ = nullptr;

	//目標スケール
	Vector3 targetScale_ = { 1.0f,1.0f,50.0f };

	//レーザーが消えている状態かどうか
	bool isLaserFading_ = false;

	//寿命
	float lifeTime_ = 4.0f;

	//死亡タイマー
	float laserFadingTimer_ = 0.0f;

	//イージング係数
	float easingParameter_ = 0.0f;

	//イージングスピード
	float easingSpeed_ = 1.0f;

	//ダメージ
	float damage_ = 1.0f;
};


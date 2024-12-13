/**
 * @file Weapon.h
 * @brief 武器を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/Primitive/TrailRenderer.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Editors/CombatAnimationEditor/AnimationEvents.h"

class Weapon : public GameObject
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

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera& camera) override;

	/// <summary>
	/// 衝突しているときの処理
	/// </summary>
	/// <param name="gameObject">衝突相手</param>
	void OnCollision(GameObject*) override;

	/// <summary>
	/// 軌跡エフェクトの有効化を設定
	/// </summary>
	/// <param name="isTrailActive">軌跡エフェクトの有効フラグ</param>
	void SetIsTrailActive(const bool isTrailActive) { isTrailActive_ = isTrailActive; };

	//ヒットフラグを取得・設定
	const bool GetIsHit() const { return isHit_; };
	void SetIsHit(const bool isHit) { isHit_ = isHit; };

	//ダメージを取得・設定
	const float GetDamage() const { return damage_; };
	void SetDamage(const float damage) { damage_ = damage; };

	//ヒットボックスのパラメーターを取得・設定
	const HitboxParameters& GetHitboxParameters() const { return hitboxParameters_; };
	void SetHitboxParameters(const HitboxParameters& hitboxParameters) { hitboxParameters_ = hitboxParameters; };

	//ノックバックのパラメーターを取得・設定
	const KnockbackParameters& GetKnockbackParameters() const { return knockbackParameters_; };
	void SetKnockbackParameters(const KnockbackParameters& knockbackParameters) { knockbackParameters_ = knockbackParameters; };

	//攻撃フラグの取得・設定
	const bool GetIsAttack() const { return isAttack_; };
	void SetIsAttack(const bool isAttack) { isAttack_ = isAttack; };

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
	/// 環境変数の初期化
	/// </summary>
	void InitializeGlobalVariables();

	/// <summary>
	/// 軌跡エフェクトの初期化
	/// </summary>
	void InitializeTrail();

	/// <summary>
	/// コライダーの更新
	/// </summary>
	void UpdateCollider();

	/// <summary>
	/// コライダーのトランスフォームを設定
	/// </summary>
	/// <param name="gameObject">ゲームオブジェクト</param>
	void SetColliderTransform(GameObject* gameObject);

	/// <summary>
	/// 軌跡エフェクトの更新
	/// </summary>
	void UpdateTrail();

	/// <summary>
	/// 環境変数の適用
	/// </summary>
	void ApplyGlobalVariables();

	/// <summary>
	/// ImGuiの更新
	/// </summary>
	void UpdateImGui();

private:
	//トランスフォーム
	TransformComponent* transform_ = nullptr;

	//モデル
	ModelComponent* model_ = nullptr;

	//コライダー
	OBBCollider* collider_ = nullptr;

	//Colliderのオフセット
	Vector3 defaultColliderOffset_{ 0.0f, 0.0f, 3.0f };

	//Colliderのサイズ
	Vector3 defaultColliderSize_{ 2.0f, 2.0f, 2.0f };

	//ダメージ
	float damage_ = 0.0f;

	//ヒットボックスのパラメーター
	HitboxParameters hitboxParameters_{};

	//ノックバックのパラメーター
	KnockbackParameters knockbackParameters_{};

	//攻撃フラグ
	bool isAttack_ = false;

	//ヒットフラグ
	bool isHit_ = false;

	//デバッグフラグ
	bool isDebug_ = false;

	//軌跡
	Trail* trail_ = nullptr;

	//軌跡のアクティブフラグ
	bool isTrailActive_ = false;

	//軌跡の頂点を追加するためのタイマー
	float addVertexTimer_ = 0.0f;

	//軌跡の頂点を追加する間隔
	float trailVertexInterval_ = 0.01f;

	//軌跡の色
	Vector4 trailStartColor_ = { 1.0f, 1.0f, 1.0f, 0.0f };
	Vector4 trailEndColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	//軌跡のオフセット値
	Vector3 headOffset_ = { 0.0f, 0.0f, -130.0f };
	Vector3 frontOffset_ = { 0.0f, 0.0f, -50.0f };
};


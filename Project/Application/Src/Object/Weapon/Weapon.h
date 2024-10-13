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

/// <summary>
/// 武器
/// </summary>
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
	void OnCollision(GameObject* gameObject) override;

	/// <summary>
	/// 軌跡エフェクトの有効化を設定
	/// </summary>
	/// <param name="isTrailActive">軌跡エフェクトの有効フラグ</param>
	void SetIsTrailActive(const bool isTrailActive) { isTrailActive_ = isTrailActive; };

	//ヒットフラグを取得
	const bool GetIsHit() const { return isHit_; };

	//エフェクトの設定の取得・設定
	const EffectSettings& GetEffectSettings() const { return effectSettings_; };
	void SetEffectSettings(const EffectSettings& effectSettings) { effectSettings_ = effectSettings; };

	//ノックバックの設定を取得・設定
	const KnockbackSettings& GetKnockbackSettings() const { return knockbackSettings_; };
	void SetKnockbackSettings(const KnockbackSettings& knockbackSettings) { knockbackSettings_ = knockbackSettings; };

	//ダメージを取得・設定
	const float GetDamage() const { return damage_; };
	void SetDamage(const float damage) { damage_ = damage; };

	//攻撃フラグの取得・設定
	const bool GetIsAttack() const { return isAttack_; };
	void SetIsAttack(const bool isAttack) { isAttack_ = isAttack; };

	//ヒットストップを設定
	void SetHitStop(HitStop* hitStop) { hitStop_ = hitStop; };

	//ヒットボックスを設定
	void SetHitbox(const Hitbox& hitbox);

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
	/// 加速フィールドの初期化
	/// </summary>
	void CreateAccelerationField();

	/// <summary>
	/// 環境変数の初期化
	/// </summary>
	void InitializeGlobalVariables();

	/// <summary>
	/// 軌跡エフェクトの初期化
	/// </summary>
	void InitializeTrail();

	/// <summary>
	/// ヒットSEの初期化
	/// </summary>
	void InitializeHitAudio();

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

	/// <summary>
	/// プレイヤーの武器が衝突した時の処理
	/// </summary>
	void HandlePlayerWeaponCollision();

	/// <summary>
	/// パーティクルのエミッターを生成
	/// </summary>
	void CreateParticleEmitters();

	/// <summary>
	/// 衝撃波のエミッターを複数生成
	/// </summary>
	/// <param name="emitterTranslation">エミッターの座標</param>
	/// <param name="camera">カメラ</param>
	void CreateShockWaveEmitters(const Vector3& emitterTranslation, Camera* camera);

	/// <summary>
	/// 火花のエミッターを生成
	/// </summary>
	/// <param name="emitterTranslation">エミッターの座標</param>
	void CreateSparkEmitter(const Vector3& emitterTranslation);

	/// <summary>
	/// 光のエミッターを生成
	/// </summary>
	/// <param name="position">エミッターの座標</param>
	void CreateLightEmitter(const Vector3& position);

	/// <summary>
	/// 衝撃波のエミッターを生成
	/// </summary>
	/// <param name="position">エミッターの座標</param>
	/// <param name="velocity">速度</param>
	/// <returns>衝撃波のエミッター</returns>
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

	//軌跡の頂点を追加するためのタイマー
	float addVertexTimer_ = 0.0f;

	//軌跡の頂点を追加する間隔
	float trailVertexInterval_ = 0.01f;

	//軌跡の色
	Vector4 trailStartColor_ = { 1.0f, 0.147f, 0.0f, 0.0f };
	Vector4 trailEndColor_ = { 1.0f, 0.147f, 0.0f, 1.0f };

	//軌跡のオフセット値
	Vector3 headOffset_ = { 0.0f, 0.0f, -130.0f };
	Vector3 frontOffset_ = { 0.0f, 0.0f, -50.0f };
};


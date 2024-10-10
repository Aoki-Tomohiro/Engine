#pragma once
#include "Engine/2D/Sprite.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Animator/AnimatorComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"
#include <array>

/// <summary>
/// キャラクターの基底クラス
/// </summary>
class ICharacter : public GameObject
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ICharacter() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    virtual void Initialize();

    /// <summary>
    /// 更新処理
    /// </summary>
    virtual void Update();

    /// <summary>
    /// UIの描画
    /// </summary>
    virtual void DrawUI() override;

    /// <summary>
    /// ダメージとノックバックを適用
    /// </summary>
    /// <param name="knockbackSettings">ノックバックの設定</param>
    /// <param name="damage">ダメージ</param>
    /// <param name="transitionToStun">スタン状態に遷移するかどうか</param>
    virtual void ApplyDamageAndKnockback(const KnockbackSettings& knockbackSettings, const float damage, const bool transitionToStun);

    /// <summary>
    /// 移動処理
    /// </summary>
    /// <param name="velocity">速度</param>
    void Move(const Vector3& velocity);

    /// <summary>
    /// 回転処理
    /// </summary>
    /// <param name="vector"></param>
    void Rotate(const Vector3& vector);

    /// <summary>
    /// ノックバックを適用
    /// </summary>
    void ApplyKnockback();

    /// <summary>
    /// 武器にパラメーターを設定
    /// </summary>
    /// <param name="weaponName">武器の名前</param>
    /// <param name="combatPhase">パラメーター</param>
    void ApplyParametersToWeapon(const std::string& weaponName, const CombatPhase& combatPhase);

    /// <summary>
    /// アニメーションによる座標のずれを補正
    /// </summary>
    void CorrectAnimationOffset();

    /// <summary>
    /// ダメージを食らった時の処理
    /// </summary>
    /// <param name="gameObject">衝突相手</param>
    /// <param name="transitionToStun">スタンに遷移するかどうか</param>
    void ProcessCollisionImpact(GameObject* gameObject, const bool transitionToStun);

    /// <summary>
    /// ジョイントのワールド座標を取得
    /// </summary>
    /// <param name="jointName">ジョイントの名前</param>
    /// <returns>ジョイントのワールド座標</returns>
    const Vector3 GetJointWorldPosition(const std::string& jointName);

    /// <summary>
    /// ジョイントのローカル座標を取得
    /// </summary>
    /// <param name="jointName">ジョイントの名前</param>
    /// <returns>ジョイントのローカル座標<</returns>
    const Vector3 GetJointLocalPosition(const std::string& jointName);

    //パーティクルエミッターの追加・削除・取得
    void AddParticleEmitter(const std::string& particleName, ParticleEmitter* emitter);
    void RemoveParticleEmitter(const std::string& particleName, const std::string& emitterName);
    ParticleEmitter* GetParticleEmitter(const std::string& particleName, const std::string& emitterName);

    //加速フィールドの追加・削除・取得
    void AddAccelerationField(const std::string& particleName, AccelerationField* field);
    void RemoveAccelerationField(const std::string& particleName, const std::string& fieldName);
    AccelerationField* GetAccelerationField(const std::string& particleName, const std::string& fieldName);

    //座標を設定・取得
    const Vector3& GetPosition() const { return transform_->worldTransform_.translation_; };
    void SetPosition(const Vector3& position) { transform_->worldTransform_.translation_ = position; };

    //クォータニオンの設定・取得
    const Quaternion& GetQuaternion() const { return transform_->worldTransform_.quaternion_; };
    void SetQuaternion(const Quaternion& quaternion) { transform_->worldTransform_.quaternion_ = quaternion; };

    //目標クォータニオンの設定・取得
    const Quaternion& GetDestinationQuaternion() const { return destinationQuaternion_; };
    void SetDestinationQuaternion(const Quaternion& destinationQuaternion) { destinationQuaternion_ = destinationQuaternion; };

    //コンバットアニメーションエディタの取得・設定
    const CombatAnimationEditor* GetCombatAnimationEditor() const { return combatAnimationEditor_; };
    void SetCombatAnimationEditor(const CombatAnimationEditor* combatAnimationEditor) { combatAnimationEditor_ = combatAnimationEditor; };

    //ノックバックを取得・設定
    const KnockbackSettings& GetKnockbackSettings() const { return knockbackSettings_; };
    void SetKnockbackSettings(const KnockbackSettings& knockbackSettings) { knockbackSettings_ = knockbackSettings; };

    //ゲームの終了状態の取得・設定
    const bool GetIsGameFinished() const { return isGameFinished_; };
    void SetIsGameFinished(const bool isGameFinished) { isGameFinished_ = isGameFinished; };

    //死亡フラグの取得
    const bool GetIsDead() const { return isDead_; };

    //重力加速度を取得
    const float GetGravityAcceleration() const { return gravityAcceleration_; };

    //アニメーション補正フラグの設定
    void SetIsAnimationCorrectionActive(const float isAnimationCorrectionActive) { isAnimationCorrectionActive_ = isAnimationCorrectionActive; };

    //回転の補間速度を設定
    void SetQuaternionInterpolationSpeed(const float quaternionInterpolationSpeed) { quaternionInterpolationSpeed_ = quaternionInterpolationSpeed; };

    //タイトルシーンのフラグを設定
    void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

    //アニメーターを取得
    AnimatorComponent* GetAnimator() const { return animator_; };

protected:
    //体力バーの種類
    enum HealthBarType
    {
        kBack,
        kFront,
    };

    //体力バーのインデックス
    enum HealthBarSegment
    {
        kLeft,
        kMiddle,
        kRight,
    };

    /// <summary>
    /// トランスフォームの初期化
    /// </summary>
    virtual void InitializeTransform();

    /// <summary>
    /// モデルの初期化
    /// </summary>
    virtual void InitializeModel();

    /// <summary>
    /// アニメーターの初期化
    /// </summary>
    virtual void InitializeAnimator();

    /// <summary>
    /// コライダーの初期化
    /// </summary>
    virtual void InitializeCollider();

    /// <summary>
    /// パーティクルシステムの初期化
    /// </summary>
    virtual void InitializeParticleSystems();

    /// <summary>
    /// UIのスプライトの初期化
    /// </summary>
    virtual void InitializeUISprites();

    /// <summary>
    /// 回転の更新処理
    /// </summary>
    virtual void UpdateRotation();

    /// <summary>
    /// コライダーの更新
    /// </summary>
    virtual void UpdateCollider();

    /// <summary>
    /// 移動制限
    /// </summary>
    virtual void RestrictMovement();

    /// <summary>
    /// HPの更新
    /// </summary>
    virtual void UpdateHP();

    /// <summary>
    /// デバッグの更新
    /// </summary>
    virtual void UpdateDebug();

    /// <summary>
    /// ImGuiの更新
    /// </summary>
    virtual void UpdateImGui();

    /// <summary>
    /// スタン状態への遷移処理
    /// </summary>
    virtual void TransitionToStunState() = 0;

protected:
    //移動制限
    const float kMoveLimit = 100.0f;

    //トランスフォーム
    TransformComponent* transform_ = nullptr;

    //クォータニオン
    Quaternion destinationQuaternion_ = Mathf::IdentityQuaternion();

    //クォータニオンの補間速度
    float quaternionInterpolationSpeed_ = 0.4f;

    //前のフレームの腰の座標
    Vector3 preAnimationHipPosition_{};

    //コライダーのオフセット値
    Vector3 colliderOffset_{};

    //モデル
    ModelComponent* model_ = nullptr;

    //アニメーション
    AnimatorComponent* animator_ = nullptr;

    //コライダー
    AABBCollider* collider_ = nullptr;

    //パーティクル関連
    std::map<std::string, ParticleSystem*> particleSystems_{};

    //コンバットアニメーションエディター
    const CombatAnimationEditor* combatAnimationEditor_ = nullptr;

    //ノックバックの設定
    KnockbackSettings knockbackSettings_{};

    //重力加速度
    float gravityAcceleration_ = -75.0f;

    //体力の最大値
    float maxHp_ = 0.0f;

    //体力
    float hp_ = 0.0f;

    //体力のスプライトの名前
    std::array<std::array<std::string, 3>, 2> hpTextureNames_{};

    //体力のスプライト
    std::array<std::array<std::unique_ptr<Sprite>, 3>, 2> hpSprites_{};

    //体力のスプライト座標
    std::array<std::array<Vector2, 3>, 2> hpBarSegmentPositions_{};

    //体力のスプライトサイズ
    Vector2 hpBarSegmentTextureSize_ = { 480.0f,18.0f };

    //アニメーションの座標補正を有効にするかどうか
    bool isAnimationCorrectionActive_ = false;

    //死亡フラグ
    bool isDead_ = false;

    //タイトルシーンにいるかどうか
    bool isInTitleScene_ = false;

    //ゲームが終了したかどうか
    bool isGameFinished_ = false;

    //デバッグのフラグ
    bool isDebug_ = false;

    //前回のデバッグのフラグ
    bool wasDebugActive_ = false;

    //アニメーションの時間
    float animationTime_ = 0.0f;

    //現在のアニメーションの名前
    std::string currentAnimationName_ = "Idle";

    //全てのアニメーションの名前
    std::vector<std::string> animationNames_{};
};


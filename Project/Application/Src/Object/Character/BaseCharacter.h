#pragma once
#include "Engine/2D/Sprite.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Animator/AnimatorComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Character/States/ICharacterState.h"
#include "Application/Src/Object/Character/States/CharacterStateFactory.h"

/// <summary>
/// キャラクターの基底クラス
/// </summary>
class BaseCharacter : public GameObject
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseCharacter() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    virtual void Initialize() override;

    /// <summary>
    /// 更新処理
    /// </summary>
    virtual void Update() override;

    /// <summary>
    /// UIの描画
    /// </summary>
    virtual void DrawUI() override;

    /// <summary>
    /// ダメージとノックバックを適用
    /// </summary>
    /// <param name="knockbackParameters">ノックバックのパラメーター</param>
    /// <param name="damage">ダメージ</param>
    /// <param name="transitionToStun">スタン状態に遷移するかどうか</param>
    virtual void ApplyDamageAndKnockback(const KnockbackParameters& knockbackParameters, const float damage, const bool transitionToStun);

    /// <summary>
    /// 状態遷移
    /// </summary>
    /// <param name="newStateName">新しい状態の名前</param>
    void ChangeState(const std::string& newStateName);

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
    /// <param name="attackEvent">攻撃イベント</param>
    void ApplyParametersToWeapon(const AttackEvent* attackEvent);

    /// <summary>
    /// ダメージを食らった時の処理
    /// </summary>
    /// <param name="gameObject">衝突相手</param>
    /// <param name="transitionToStun">スタンに遷移するかどうか</param>
    void ProcessCollisionImpact(GameObject* gameObject, const bool transitionToStun);

    /// <summary>
    /// モデルシェイク開始
    /// </summary>
    void StartModelShake();

    /// <summary>
    /// デバッグモードの開始
    /// </summary>
    /// <param name="animationName">アニメーションの名前</param>
    void StartDebugMode(const std::string& animationName);

    /// <summary>
    /// デバッグモードの終了
    /// </summary>
    void EndDebugMode();

    /// <summary>
    /// ジョイントのワールド座標を取得
    /// </summary>
    /// <param name="jointName">ジョイントの名前</param>
    /// <returns>ジョイントのワールド座標</returns>
    const Vector3 GetJointWorldPosition(const std::string& jointName) const;

    /// <summary>
    /// ジョイントのローカル座標を取得
    /// </summary>
    /// <param name="jointName">ジョイントの名前</param>
    /// <returns>ジョイントのローカル座標<</returns>
    const Vector3 GetJointLocalPosition(const std::string& jointName) const;

    //座標を設定・取得
    const Vector3& GetPosition() const { return transform_->worldTransform_.translation_; };
    void SetPosition(const Vector3& position) { transform_->worldTransform_.translation_ = position; };

    //クォータニオンの設定・取得
    const Quaternion& GetQuaternion() const { return transform_->worldTransform_.quaternion_; };
    void SetQuaternion(const Quaternion& quaternion) { transform_->worldTransform_.quaternion_ = quaternion; };

    //目標クォータニオンの設定・取得
    const Quaternion& GetDestinationQuaternion() const { return destinationQuaternion_; };
    void SetDestinationQuaternion(const Quaternion& destinationQuaternion) { destinationQuaternion_ = destinationQuaternion; };

    //ノックバックのパラメーターを取得・設定
    const KnockbackParameters& GetKnockbackParameters() const { return knockbackParameters_; };
    void SetKnockbackParameters(const KnockbackParameters& knockbackParameters) { knockbackParameters_ = knockbackParameters; };

    //ゲームの終了状態の取得・設定
    const bool GetIsGameFinished() const { return isGameFinished_; };
    void SetIsGameFinished(const bool isGameFinished) { isGameFinished_ = isGameFinished; };

    //武器を設定・取得
    Weapon* GetWeapon() const { return weapon_; };
    void SetWeapon(Weapon* weapon) { weapon_ = weapon; };

    //エディターマネージャーの取得・設定
    const EditorManager* GetEditorManager() const { return editorManager_; };
    void SetEditorManager(const EditorManager* editorManager) { editorManager_ = editorManager; };

    //重力加速度を取得
    const float GetGravityAcceleration() const { return gravityAcceleration_; };

    //デバッグのフラグの設定・取得
    const bool GetIsDebug() const { return isDebug_; };

    //死亡フラグの取得
    const bool GetIsDead() const { return isDead_; };

    //回転の補間速度を設定
    void SetQuaternionInterpolationSpeed(const float quaternionInterpolationSpeed) { quaternionInterpolationSpeed_ = quaternionInterpolationSpeed; };

    //タイトルシーンのフラグを設定
    void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

    //ヒットストップを設定
    void SetHitStop(HitStop* hitStop) { hitStop_ = hitStop; };

    //アニメーターを取得
    AnimatorComponent* GetAnimator() const { return animator_; };

protected:
    //モデルシェイク用の構造体
    struct ModelShake
    {
        bool isActive = false;                   // モデルシェイクが有効かどうか
        float duration = 0.1f;                   // シェイクの継続時間
        float elapsedTime = 0.0f;                // 経過したシェイク時間
        Vector3 originalPosition{};              // 元のモデルの位置
        Vector3 intensity{ 30.0f, 0.0f, 30.0f }; // シェイクの強度
    };

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
    /// UIのスプライトの初期化
    /// </summary>
    virtual void InitializeUISprites();

    /// <summary>
    /// 武器の初期化
    /// </summary>
    virtual void InitializeWeapon();

    /// <summary>
    /// 環境変数の初期化
    /// </summary>
    virtual void InitializeGlobalVariables();

    /// <summary>
    /// 回転の更新処理
    /// </summary>
    void UpdateRotation();

    /// <summary>
    /// コライダーの更新
    /// </summary>
    void UpdateCollider();

    /// <summary>
    /// 移動制限
    /// </summary>
    void RestrictMovement();

    /// <summary>
    /// HPの更新
    /// </summary>
    void UpdateHP();

    /// <summary>
    /// 死亡状態に遷移するかを確認
    /// </summary>
    void CheckAndTransitionToDeath();

    /// <summary>
    /// 次の状態に遷移
    /// </summary>
    void TransitionToNextState();

    /// <summary>
    /// モデルシェイクの更新
    /// </summary>
    void UpdateModelShake();

    /// <summary>
    /// モデルシェイクを適用
    /// </summary>
    void ApplyModelShake();

    /// <summary>
    /// モデルシェイクでズレた分の座標をリセット
    /// </summary>
    void ResetToOriginalPosition();

    /// <summary>
    /// 環境変数の適用
    /// </summary>
    virtual void ApplyGlobalVariables();

protected:
    //移動制限
    const float kMoveLimit = 100.0f;

    //現在の状態
    std::unique_ptr<ICharacterState> currentState_ = nullptr;

    //新しい状態
    std::unique_ptr<ICharacterState> nextState_ = nullptr;

    //キャラクターの新しい状態を生成するファクトリー
    CharacterStateFactory* characterStateFactory_ = nullptr;

    //トランスフォーム
    TransformComponent* transform_ = nullptr;

    //モデル
    ModelComponent* model_ = nullptr;

    //アニメーター
    AnimatorComponent* animator_ = nullptr;

    //コライダー
    AABBCollider* collider_ = nullptr;

    //武器
    Weapon* weapon_ = nullptr;

    //ヒットストップ
    HitStop* hitStop_ = nullptr;

    //クォータニオン
    Quaternion destinationQuaternion_ = Mathf::IdentityQuaternion();

    //クォータニオンの補間速度
    float quaternionInterpolationSpeed_ = 0.4f;

    //コライダーのオフセット値
    Vector3 colliderOffset_{};

    //ノックバックのパラメーター
    KnockbackParameters knockbackParameters_{};

    //モデルシェイク
    ModelShake modelShake_{};

    //重力加速度
    float gravityAcceleration_ = -180.0f;

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

    //タイトルシーンにいるかどうか
    bool isInTitleScene_ = false;

    //死亡フラグ
    bool isDead_ = false;

    //ゲームが終了したかどうか
    bool isGameFinished_ = false;

    //デバッグのフラグ
    bool isDebug_ = false;

    //エディターマネージャー
    const EditorManager* editorManager_ = nullptr;
};


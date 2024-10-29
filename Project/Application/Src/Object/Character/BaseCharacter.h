#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Animator/AnimatorComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Weapon/Weapon.h"

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
    virtual void Initialize();

    /// <summary>
    /// 移動処理
    /// </summary>
    /// <param name="velocity">速度</param>
    void Move(const Vector3& velocity);

    /// <summary>
    /// 武器にパラメーターを設定
    /// </summary>
    /// <param name="weaponName">武器の名前</param>
    /// <param name="attackEvent">攻撃イベント</param>
    void ApplyParametersToWeapon(const AttackEvent* attackEvent);

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

    //座標を取得・設定
    const Vector3& GetPosition() const { return transform_->worldTransform_.translation_; };
    void SetPosition(const Vector3& position) { transform_->worldTransform_.translation_ = position; };

    //クォータニオンの取得・設定
    const Quaternion& GetQuaternion() const { return transform_->worldTransform_.quaternion_; };
    void SetQuaternion(const Quaternion& quaternion) { transform_->worldTransform_.quaternion_ = quaternion; };

    //武器を取得・設定
    Weapon* GetWeapon() const { return weapon_; };
    void SetWeapon(Weapon* weapon) { weapon_ = weapon; };

    //タイトルシーンのフラグを設定
    void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

    //デバッグのフラグの設定・取得
    const bool GetIsDebug() const { return isDebug_; };

    //アニメーターを取得
    AnimatorComponent* GetAnimator() const { return animator_; };

protected:
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

protected:
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

    //クォータニオン
    Quaternion destinationQuaternion_ = Mathf::IdentityQuaternion();

    //クォータニオンの補間速度
    float quaternionInterpolationSpeed_ = 0.4f;

    //タイトルシーンにいるかどうか
    bool isInTitleScene_ = false;

    //デバッグのフラグ
    bool isDebug_ = false;
};


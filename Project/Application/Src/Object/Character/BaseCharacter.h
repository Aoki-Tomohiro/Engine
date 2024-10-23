#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Animator/AnimatorComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Math/MathFunction.h"

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

    //タイトルシーンのフラグを設定
    void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

private:
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

private:
    //トランスフォーム
    TransformComponent* transform_ = nullptr;

    //モデル
    ModelComponent* model_ = nullptr;

    //アニメーション
    AnimatorComponent* animator_ = nullptr;

    //コライダー
    AABBCollider* collider_ = nullptr;

    //クォータニオン
    Quaternion destinationQuaternion_ = Mathf::IdentityQuaternion();

    //クォータニオンの補間速度
    float quaternionInterpolationSpeed_ = 0.4f;

    //タイトルシーンにいるかどうか
    bool isInTitleScene_ = false;
};


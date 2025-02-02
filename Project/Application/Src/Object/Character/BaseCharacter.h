/**
 * @file BaseCharacter.h
 * @brief キャラクターの基底クラスを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/2D/Sprite.h"
#include "Engine/3D/Model/AnimationManager.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Animator/AnimatorComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/HitStop/HitStop.h"
#include "Application/Src/Object/Camera/CameraController.h"
#include "Application/Src/Object/Character/States/AbstractCharacterState.h"
#include "Application/Src/Object/Character/States/CharacterStateFactory.h"
#include "Application/Src/Object/Editors/EditorManager.h"
#include "Application/Src/Object/UI/UIManager.h"

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
    /// <returns>遷移したかどうか</returns>
    bool ChangeState(const std::string& newStateName);

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
    /// サウンドエフェクトを再生
    /// </summary>
    /// <param name="soundEffectType">サウンドエフェクトのタイプ</param>
    void PlaySoundEffect(const std::string& soundEffectType);

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
    Vector3 GetJointWorldPosition(const std::string& jointName) const;

    /// <summary>
    /// ジョイントのローカル座標を取得
    /// </summary>
    /// <param name="jointName">ジョイントの名前</param>
    /// <returns>ジョイントのローカル座標<</returns>
    Vector3 GetJointLocalPosition(const std::string& jointName) const;

    /// <summary>
    /// アクションが実行可能かどうかを返す（状態条件とトリガー条件の両方）
    /// </summary>
    /// <param name="actionName">アクション名</param>
    /// <returns>アクションが実行可能かどうか（状態条件とトリガー条件の両方）</returns>
    const bool IsActionExecutable(const std::string& actionName) const { return actionMap_.count(actionName) ? actionMap_.at(actionName).stateCondition() && actionMap_.at(actionName).triggerCondition() : false; };

    /// <summary>
    /// アクションが実行可能かどうかを返す（状態条件のみ）
    /// </summary>
    /// <param name="actionName">アクション名</param>
    /// <returns>アクションが実行可能かどうか（状態条件のみ）</returns>
    const bool GetActionStateCondition(const std::string& actionName) const { return actionMap_.count(actionName) ? actionMap_.at(actionName).stateCondition() : false; };

    /// <summary>
    /// アクションが実行可能かどうかを返す（トリガー条件のみ）
    /// </summary>
    /// <param name="actionName">アクション名</param>
    /// <returns>アクションが実行可能かどうか（トリガー条件のみ）</returns>
    const bool GetActionTriggerCondition(const std::string& actionName) const { return actionMap_.count(actionName) ? actionMap_.at(actionName).triggerCondition() : false; };
    
    //原点を取得・設定
    const Vector3& GetOriginOffset() const { return transform_->worldTransform_.originOffset_; };
    void SetOriginnOffset(const Vector3& originOffset) { transform_->worldTransform_.originOffset_ = originOffset; };

    //座標を取得・設定
    const Vector3& GetPosition() const { return transform_->worldTransform_.translation_; };
    void SetPosition(const Vector3& position) { transform_->worldTransform_.translation_ = position; };

    //クォータニオンの取得・設定
    const Quaternion& GetQuaternion() const { return transform_->worldTransform_.quaternion_; };
    void SetQuaternion(const Quaternion& quaternion) { transform_->worldTransform_.quaternion_ = quaternion; };

    //目標クォータニオンの取得・設定
    const Quaternion& GetDestinationQuaternion() const { return destinationQuaternion_; };
    void SetDestinationQuaternion(const Quaternion& destinationQuaternion) { destinationQuaternion_ = destinationQuaternion; };

    //ノックバックのパラメーターを取得・設定
    const KnockbackParameters& GetKnockbackParameters() const { return knockbackParameters_; };
    void SetKnockbackParameters(const KnockbackParameters& knockbackParameters) { knockbackParameters_ = knockbackParameters; };

    //ジャスト回避が可能かどうかを取得・設定
    const bool GetIsVulnerableToPerfectDodge() const { return isVulnerableToPerfectDodge_; };
    void SetIsVulnerableToPerfectDodge(const bool isVulnerableToPerfectDodge) { isVulnerableToPerfectDodge_ = isVulnerableToPerfectDodge; };

    //スタン状態に遷移したかどうかを取得・設定
    const bool GetIsStunTriggered() const { return isStunTriggered_; };
    void SetIsStunTriggered(const bool isStunTriggered) { isStunTriggered_ = isStunTriggered; };

    //吹き飛ばされたかどうかを取得・設定
    const bool GetIsKnockedBack() const { return isKnockedBack_; };
    void SetIsKnockback(const bool isKnockedBack) { isKnockedBack_ = isKnockedBack; };

    //ゲームの終了状態の取得・設定
    const bool GetIsGameFinished() const { return isGameFinished_; };
    void SetIsGameFinished(const bool isGameFinished) { isGameFinished_ = isGameFinished; };

    //タイムスケールの取得・設定
    const float GetTimeScale() const { return timeScale_; };
    void SetTimeScale(const float timeScale) { timeScale_ = timeScale; };

    //武器を取得・設定
    Weapon* GetWeapon() const { return weapon_; };
    void SetWeapon(Weapon* weapon) { weapon_ = weapon; };

    //ヒットストップを取得・設定
    HitStop* GetHitStop() const { return hitStop_; };
    void SetHitStop(HitStop* hitStop) { hitStop_ = hitStop; };

    //カメラコントローラーの取得・設定
    CameraController* GetCameraController() const { return cameraController_; };
    void SetCameraController(CameraController* cameraController) { cameraController_ = cameraController; };

    //UIマネージャーの取得・設定
    UIManager* GetUIManager() const { return uiManager_; };
    void SetUIManager(UIManager* uiManager) { uiManager_ = uiManager; };

    //エディターマネージャーの取得・設定
    const EditorManager* GetEditorManager() const { return editorManager_; };
    void SetEditorManager(const EditorManager* editorManager) { editorManager_ = editorManager; };

    //アクションマップのキーを取得
    const std::vector<std::string> GetActionKeys() const { return GetKeysFromMap(actionMap_); };

    //オーディオハンドルのキーを取得
    const std::vector<std::string> GetAudioHandles() const { return GetKeysFromMap(audioHandles_); };

    //QTEのアクションを取得
    const std::vector<std::string>& GetQTEActions() const { return qteActions_; };

    //重力加速度を取得
    const float GetGravityAcceleration() const { return gravityAcceleration_; };

    //地面のY座標を取得
    const float GetAdjustGroundLevel() const { return GetOriginOffset().y * -1.0f; };

    //デバッグのフラグの設定・取得
    const bool GetIsDebug() const { return isDebug_; };

    //死亡フラグの取得
    const bool GetIsDead() const { return isDead_; };

    //回転の補間速度を設定
    void SetQuaternionInterpolationSpeed(const float quaternionInterpolationSpeed) { quaternionInterpolationSpeed_ = quaternionInterpolationSpeed; };

    //タイトルシーンのフラグを設定
    void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

    //アニメーターを取得
    AnimatorComponent* GetAnimator() const { return animator_; };

    //コライダーを取得
    AABBCollider* GetCollider() const { return collider_; };

protected:
    //アクションの実行条件を格納する構造体
    struct ActionCondition
    {
        std::function<bool()> stateCondition;   //ゲームの状態やキャラクターの状況に基づく追加条件
        std::function<bool()> triggerCondition; //アクションをトリガーするための条件（ボタン入力やその他の条件）
    };

    //モデルシェイク用の構造体
    struct ModelShake
    {
        bool isActive = false;                   // モデルシェイクが有効かどうか
        float duration = 0.1f;                   // シェイクの継続時間
        float elapsedTime = 0.0f;                // 経過したシェイク時間
        Vector3 originalPosition{};              // 元のモデルの位置
        Vector3 intensity{ 30.0f, 0.0f, 30.0f }; // シェイクの強度
    };

    /// <summary>
    /// アクションマップの初期化
    /// </summary>
    virtual void InitializeActionMap() = 0;

    /// <summary>
    /// オーディオの初期化
    /// </summary>
    virtual void InitializeAudio();

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
    /// 環境変数の初期化
    /// </summary>
    virtual void InitializeGlobalVariables();

    /// <summary>
    /// 環境変数の適用
    /// </summary>
    virtual void ApplyGlobalVariables();

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
    /// 押し戻し処理
    /// </summary>
    /// <param name="transform1">一つ目のトランスフォーム</param>
    /// <param name="transform2">二つ目のトランスフォーム</param>
    /// <param name="collider1">一つ目のコライダー</param>
    /// <param name="collider2">二つ目のコライダー</param>
    void ResolveCollision(TransformComponent* transform1, TransformComponent* transform2, AABBCollider* collider1, AABBCollider* collider2);

    /// <summary>
    /// CSVファイルからリソースデータを読み込み、指定されたマップに保存する関数
    /// </summary>
    /// <param name="filePath">ファイルパス</param>
    /// <param name="resourceMap">読み込んだリソースのキーとデータを格納するマップ</param>
    void LoadResourceFromCSV(const std::string& filePath, std::map<std::string, std::string>& resourceMap);

    /// <summary>
    /// 指定されたコンテナのキーを取り出し配列として返す関数
    /// </summary>
    /// <typeparam name="Type">キーと値のペアで構成されるコンテナ型</typeparam>
    /// <param name="map">キーと値のペアを格納するコンテナ</param>
    /// <returns>コンテナ内のすべてのキーを格納した配列</returns>
    template <typename Type>
    std::vector<std::string> GetKeysFromMap(const Type& map) const;

protected:
    //移動制限
    const float kMoveLimit_ = 100.0f;

    //体力の最大値
    const float kMaxHp_ = 100.0f;

    //重力加速度
    const float gravityAcceleration_ = -110.0f;

    //オーディオ
    Audio* audio_ = nullptr;

    //現在の状態
    std::unique_ptr<AbstractCharacterState> currentState_ = nullptr;

    //新しい状態
    std::unique_ptr<AbstractCharacterState> nextState_ = nullptr;

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

    //アクションに対応する条件式を保持するマップ
    std::map<std::string, ActionCondition> actionMap_{};

    //オーディオハンドル
    std::map<std::string, uint32_t> audioHandles_{};

    //QTEのアクション
    std::vector<std::string> qteActions_{};

    //タイムスケール
    float timeScale_ = 1.0f;

    //体力
    float hp_ = kMaxHp_;

    //体力バーの真ん中のスプライト
    DynamicUI* hpBarMidSprite_ = nullptr;

    //体力バーの右のスプライト
    UIElement* hpBarRightSprite_ = nullptr;

    //ジャスト回避が可能かどうか
    bool isVulnerableToPerfectDodge_ = false;

    //スタン状態に遷移したかどうか
    bool isStunTriggered_ = false;

    //吹き飛ばされたかどうか
    bool isKnockedBack_ = false;

    //タイトルシーンにいるかどうか
    bool isInTitleScene_ = false;

    //死亡フラグ
    bool isDead_ = false;

    //ゲームが終了したかどうか
    bool isGameFinished_ = false;

    //デバッグのフラグ
    bool isDebug_ = false;

    //カメラコントローラー
    CameraController* cameraController_ = nullptr;

    //UIマネージャー
    UIManager* uiManager_ = nullptr;

    //エディターマネージャー
    const EditorManager* editorManager_ = nullptr;
};


template<typename Type>
inline std::vector<std::string> BaseCharacter::GetKeysFromMap(const Type& map) const
{
    //キーを格納するための配列
    std::vector<std::string> keys;
    //キーをすべて追加
    for (const auto& pair : map)
    {
        keys.push_back(pair.first);
    }
    //キーを格納した配列を返す
    return keys;
}

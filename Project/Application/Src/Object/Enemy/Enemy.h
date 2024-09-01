#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Enemy/States/IEnemyState.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"
#include <numbers>

class Enemy : public GameObject
{
public:
	//通常状態のパラメーター
	struct RootParameters
	{
		float walkSpeed = 7.0f;           // 歩き移動速度
		float runSpeed = 20.0f;           // 走り移動速度
		float minActionInterval_ = 2.0f;  // 攻撃までの最小時間
		float maxActionInterval_ = 4.0f;  // 攻撃までの最大時間
		float stopDistance = 4.0f;        // 動きを止める距離
		float closeRangeDistance = 20.0f; // 遠距離の行動をする距離
	};

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void ChangeState(IEnemyState* newState);

	void Move(const Vector3& velocity);

	void Rotate(const Vector3& vector);

	void CheckAndTransitionToDeath();

	void ApplyKnockback();

	void CorrectAnimationOffset();

	void PlayAnimation(const std::string& name, const float speed, const bool loop);

	void StopAnimation();

	void SetIsAnimationBlending(bool isBlending);

	float GetCurrentAnimationTime();

	float GetNextAnimationTime();

	const bool GetIsAnimationFinished();

	const bool GetIsBlendingCompleted();

	const Vector3 GetHipLocalPosition();

	const Vector3 GetHipWorldPosition();

	const Vector3 GetPosition();

	void SetPosition(const Vector3& position);

	const Quaternion& GetDestinationQuaternion() const { return destinationQuaternion_; };

	void SetDestinationQuaternion(const Quaternion& destinationQuaternion) { destinationQuaternion_ = destinationQuaternion; };

	const float GetTimeScale() const { return timeScale_; };

	void SetTimeScale(const float timeScale) { timeScale_ = timeScale; };

	const float GetHP() const { return hp_; };

	void SetHP(const float hp) { hp_ = hp; };

	void SetIsAnimationCorrectionActive(const float isAnimationCorrectionActive) { isAnimationCorrectionActive_ = isAnimationCorrectionActive; };

	void SetKnockbackSettings(const KnockbackSettings& knockbackSettings) { knockbackSettings_ = knockbackSettings; };

	const bool GetIsDead() const { return isDead_; };

	void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

	const bool GetIsGameFinished() const { return isGameFinished_; };

	void SetIsGameFinished(const bool isGameFinished) { isGameFinished_ = isGameFinished; };

	const CombatAnimationEditor* GetCombatAnimationEditor() const { return combatAnimationEditor_; };

	void SetCombatAnimationEditor(const CombatAnimationEditor* combatAnimationEditor) { combatAnimationEditor_ = combatAnimationEditor; };

	const RootParameters& GetRootParameters() const { return rootParameters_; };

private:
	void UpdateRotation();

	void UpdateCollider();

	void RestrictPlayerMovement(float moveLimit);

	void UpdateHP();

	void DebugUpdate();

	void ApplyGlobalVariables();

	void UpdateImGui();

private:
	//状態
	std::unique_ptr<IEnemyState> state_ = nullptr;

	//Quaternion
	Quaternion destinationQuaternion_ = Mathf::IdentityQuaternion();

	//回転の補間速度
	float quaternionInterpolationSpeed_ = 0.4f;

	//前のフレームのアニメーション後の座標
	Vector3 preAnimationHipPosition_{};

	//コライダーのオフセット値
	Vector3 colliderOffset_{};

	//CombatAnimationEditor
	const CombatAnimationEditor* combatAnimationEditor_ = nullptr;

	//通常状態のパラメーター
	RootParameters rootParameters_{};

	//ノックバックの設定
	KnockbackSettings knockbackSettings_{};

	//重力加速度
	float gravityAcceleration = -42.0f;

	//タイムスケール
	float timeScale_ = 1.0f;

	//アニメーションの座標のずれを補正したかどうか
	bool isAnimationCorrectionActive_ = false;

	//体力
	const float kMaxHP = 800.0f;
	float hp_ = kMaxHP;

	//体力のスプライト
	std::unique_ptr<Sprite> hpSprite_ = nullptr;
	std::unique_ptr<Sprite> hpFrameSprite_ = nullptr;
	Vector2 hpSpritePosition_ = { 720.0f,32.0f };
	Vector2 hpSpriteSize_{ 480.0f,16.0f };
	Vector2 hpFrameSpritePosition_ = { 719.0f,31.0f };

	//死亡フラグ
	bool isDead_ = false;

	//ゲームが終了したかどうか
	bool isGameFinished_ = false;

	//タイトルシーンなのか
	bool isInTitleScene_ = false;

	//デバッグ用のフラグ
	bool isDebug_ = false;

	//アニメーション時間
	float animationTime_ = 0.0f;

	//現在のアニメーション
	std::string currentAnimationName_ = "Idle";

	//アニメーション一覧
	std::vector<std::string> animationName_ = { {"Idle"}, {"Walk"}, {"Run"}, {"Stun"}, {"TackleAttack"}, {"JumpAttack"}, {"ComboAttack"} };
};


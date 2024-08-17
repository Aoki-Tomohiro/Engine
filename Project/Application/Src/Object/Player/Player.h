#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Lockon/Lockon.h"
#include "Application/Src/Object/Player/States/IPlayerState.h"
#include "Application/Src/Object/AnimationStateManager/AnimationStateManager.h"
#include <numbers>

class Player : public GameObject
{
public:
	//アクションフラグ
	enum class ActionFlag
	{
		kDashing,
		kFallingAttack,
		kBackhandAttack,
	};

	//通常状態のパラメーター
	struct RootParameters
	{
		float walkThreshold = 0.3f; //歩きの閾値
		float runThreshold = 0.6f;  //走りの閾値
		float walkSpeed = 9.0f;     //歩きの移動速度
		float runSpeed = 18.0f;     //走りの移動速度
	};

	//ジャンプ状態のパラメーター
	struct JumpParameters
	{
		float jumpFirstSpeed = 35.0f;      //初速度
		float gravityAcceleration = -2.8f; //重力加速度
	};

	//回避用のパラメーター
	struct DodgeParameters
	{
		float dodgeSpeed = 21.0f; //回避速度
	};

	//ダッシュ用のパラメーター
	struct DashParameters
	{
		float dashSpeed = 105.0f;       //ダッシュ速度
		float proximityDistance = 8.0f; //移動を止める距離
	};

	//地上攻撃用のパラメーター
	struct AttackParameters
	{
		int32_t comboNum = 3;                                    //コンボの数
		float attackDistance = 6.0f;                             //攻撃の補正を掛ける距離
		std::vector<float> moveSpeeds = { 9.0f,9.0f,9.0f };      //移動速度
		float gravityAcceleration = -2.8f;                       //攻撃中の重力加速度
	};

	//落下攻撃用のパラメーター
	struct FallingAttackParameters
	{
		float fallingSpeed = -25.0f;       //落下速度
		float fallingAcceleration = -2.8f; //落下加速度
	};

	//バックハンド攻撃用のパラメーター
	struct BackhandAttackParameters
	{
		int32_t kMaxHitCount = 5;
		float hitInterval = 0.02f;
	};

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void ChangeState(IPlayerState* newState);

	void DebugUpdate();

	void Move(const Vector3& velocity);

	void Rotate(const Vector3& vector);

	void CorrectAnimationOffset();

	void LookAtEnemy();

	const bool GetIsAnimationEnd();

	const float GetAnimationTime();

	const float GetAnimationSpeed();

	void SetAnimationSpeed(const float animationSpeed);

	void SetAnimationTime(const float animationTime);

	void SetAnimationName(const std::string& animationName);

	void SetIsAnimationLoop(const bool isAnimationLoop);

	void SetIsAnimationStop(const bool isAnimationStop);

	const Quaternion& GetDestinationQuaternion() const { return destinationQuaternion_; };

	const Vector3 GetPosition();

	void SetPosition(const Vector3& position);

	const Vector3 GetHipLocalPosition();

	const Vector3 GetHipWorldPosition();

	bool GetActionFlag(const ActionFlag& flag) const
	{
		auto it = flags_.find(flag);
		return it != flags_.end() && it->second;
	}

	void SetActionFlag(const ActionFlag& flag, bool value) { flags_[flag] = value; };

	const Camera* GetCamera() const { return camera_; };

	void SetCamera(const Camera* camera) { camera_ = camera; };

	const Lockon* GetLockon() const { return lockon_; };

	void SetLockon(const Lockon* lockOn) { lockon_ = lockOn; }

	const AnimationStateManager* GetAnimationStateManager() const { return animationStateManager_; };

	void SetAnimationStateManager(const AnimationStateManager* animationStateManager) { animationStateManager_ = animationStateManager; };

	const RootParameters& GetRootParameters() const { return rootParameters_; };

	const JumpParameters& GetJumpParameters() const { return jumpParameters_; };

	const DodgeParameters& GetDodgeParameters() const { return dodgeParameters_; };

	const DashParameters& GetDashParameters() const { return dashParameters_; };

	const AttackParameters& GetAttackParameters() const { return attackParameters_; };

	const FallingAttackParameters& GetFallingAttackParameters() const { return fallingAttackParameters_; };

	const BackhandAttackParameters& GetBackhandAttackParameters() const { return backhandAttackParameters_; };

private:
	void ApplyGlobalVariables();

private:
	//Input
	Input* input_ = nullptr;

	//Audio
	Audio* audio_ = nullptr;

	//状態
	std::unique_ptr<IPlayerState> state_ = nullptr;

	//Quaternion
	Quaternion destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>);

	//回転の補間速度
	float quaternionInterpolationSpeed_ = 0.4f;

	//アニメーション後の座標
	Vector3 preAnimationHipPosition_{};

	//コライダーのオフセット値
	Vector3 colliderOffset_{};

	//アクションフラグ
	std::unordered_map<ActionFlag, bool> flags_;

	//Camera
	const Camera* camera_ = nullptr;

	//LockOn
	const Lockon* lockon_ = nullptr;

	//AnimationPhaseManager
	const AnimationStateManager* animationStateManager_ = nullptr;

	//デバッグ用のフラグ
	bool isDebug_ = false;

	//アニメーション時間
	float animationTime_ = 0.0f;

	//通常状態のパラメーター
	RootParameters rootParameters_{};

	//ジャンプ状態のパラメーター
	JumpParameters jumpParameters_{};

	//回避状態のパラメーター
	DodgeParameters dodgeParameters_{};

	//ダッシュ用のパラメーター
	DashParameters dashParameters_{};

	//地上攻撃用のパラメーター
	AttackParameters attackParameters_{};

	//落下攻撃用のパラメーター
	FallingAttackParameters fallingAttackParameters_{};

	//バックハンド攻撃用のパラメーター
	BackhandAttackParameters backhandAttackParameters_{};

	//アニメーション一覧
	std::vector<std::string> animationName_ = {
		{"Armature|mixamo.com|Layer0"},
		{"Armature.001|mixamo.com|Layer0"},
		{"Armature.001|mixamo.com|Layer0.001"},
		{"Armature.001|mixamo.com|Layer0.002"},
		{"Armature.001|mixamo.com|Layer0.003"},
		{"Armature.001|mixamo.com|Layer0.004"},
		{"Armature.001|mixamo.com|Layer0.005"},
		{"Armature.001|mixamo.com|Layer0.006"},
		{"Armature.001|mixamo.com|Layer0.007"},
		{"Armature.001|mixamo.com|Layer0.008"},
		{"Armature.001|mixamo.com|Layer0.009"},
		{"Armature.001|mixamo.com|Layer0.010"},
		{"Armature.001|mixamo.com|Layer0.011"},
		{"Armature.001|mixamo.com|Layer0.012"},
		{"Armature.001|mixamo.com|Layer0.013"},
		{"Armature.001|mixamo.com|Layer0.014"},
		{"Armature.001|mixamo.com|Layer0.015"},
		{"Armature.001|mixamo.com|Layer0.016"},
		{"Armature.001|mixamo.com|Layer0.017"},
		{"Armature.001|mixamo.com|Layer0.018"},
		{"Armature.001|mixamo.com|Layer0.019"},
		{"Armature.001|mixamo.com|Layer0.020"},
		{"Armature.001|mixamo.com|Layer0.021"},
		{"Armature.001|mixamo.com|Layer0.022"},
		{"Armature.001|mixamo.com|Layer0.023"},
	};

	//現在のアニメーション
	std::string currentAnimationName_ = animationName_[0];
};


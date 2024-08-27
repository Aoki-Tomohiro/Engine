#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Lockon/Lockon.h"
#include "Application/Src/Object/AnimationStateManager/AnimationStateManager.h"
#include "Application/Src/Object/Player/States/IPlayerState.h"
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
		float walkSpeed = 9.0f;     //歩きの移動速度
		float runThreshold = 0.6f;  //走りの閾値
		float runSpeed = 18.0f;     //走りの移動速度
	};

	//ジャンプ状態のパラメーター
	struct JumpParameters
	{
		float jumpFirstSpeed = 25.0f;      //初速度
		float gravityAcceleration = -2.8f; //重力加速度
	};

	//回避用のパラメーター
	struct DodgeParameters
	{
		float dodgeDistance = 10.0f; //回避速度
	};

	//ダッシュ用のパラメーター
	struct DashParameters
	{
		float dashSpeed = 105.0f;       //ダッシュ速度
		float proximityDistance = 10.0f; //移動を止める距離
	};

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void ChangeState(IPlayerState* newState);

	void Move(const Vector3& velocity);

	void Rotate(const Vector3& vector);

	void LookAtEnemy();

	void CorrectAnimationOffset();

	void PlayAnimation(const std::string& name, const float speed, const bool loop);

	void StopAnimation();

	void SetIsAnimationBlending(const bool isBlending);

	void SetAnimationBlendDuration(const float blendDuration);

	const float GetAnimationTime();

	const float GetAnimationDuration();

	const bool GetIsAnimationFinished();

	const Vector3 GetHipLocalPosition();

	const Vector3 GetHipWorldPosition();

	const Vector3 GetPosition();

	void SetPosition(const Vector3& position);

	const Quaternion& GetDestinationQuaternion() const { return destinationQuaternion_; };

	void SetDestinationQuaternion(const Quaternion& destinationQuaternion) { destinationQuaternion_ = destinationQuaternion; };

	bool GetActionFlag(const ActionFlag& flag) const
	{
		auto it = flags_.find(flag);
		return it != flags_.end() && it->second;
	}

	void SetActionFlag(const ActionFlag& flag, bool value) { flags_[flag] = value; };
	
	void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };
	
	void AddParticleEmitter(const std::string& name, ParticleEmitter* particleEmitter);

	void RemoveParticleEmitter(const std::string& particleName, const std::string& emitterName);

	void AddAccelerationField(const std::string& name, AccelerationField* accelerationField);

	void RemoveAccelerationField(const std::string& particleName, const std::string& accelerationFieldName);

	ParticleSystem* GetParticleSystem(const std::string& name) const;

	const Camera* GetCamera() const { return camera_; };

	void SetCamera(const Camera* camera) { camera_ = camera; };

	const Lockon* GetLockon() const { return lockon_; };

	void SetLockon(const Lockon* lockOn) { lockon_ = lockOn; };

	const AnimationStateManager* GetAnimationStateManager() const { return animationStateManager_; };

	void SetAnimationStateManager(const AnimationStateManager* animationStateManager) { animationStateManager_ = animationStateManager; };

	const RootParameters& GetRootParameters() const { return rootParameters_; };

	const JumpParameters& GetJumpParameters() const { return jumpParameters_; };

	const DodgeParameters& GetDodgeParameters() const { return dodgeParameters_; };

	const DashParameters& GetDashParameters() const { return dashParameters_; };

private:
	void InitializeParticleSystems();

	void UpdateRotation();

	void UpdateCollider();

	void DebugUpdate();

	void ApplyGlobalVariables();

	void UpdateImGui();

private:
	//インプット
	Input* input_ = nullptr;

	//オーディオ
	Audio* audio_ = nullptr;

	//状態
	std::unique_ptr<IPlayerState> state_ = nullptr;

	//Quaternion
	Quaternion destinationQuaternion_ = Mathf::IdentityQuaternion();

	//回転の補間速度
	float quaternionInterpolationSpeed_ = 0.4f;

	//前のフレームのアニメーション後の座標
	Vector3 preAnimationHipPosition_{};

	//コライダーのオフセット値
	Vector3 colliderOffset_{};

	//アクションフラグ
	std::unordered_map<ActionFlag, bool> flags_{};

	//パーティクル
	std::map<std::string, ParticleSystem*> particleSystems_{};

	//カメラ
	const Camera* camera_ = nullptr;

	//ロックオン
	const Lockon* lockon_ = nullptr;

	//AnimationPhaseManager
	const AnimationStateManager* animationStateManager_ = nullptr;

	//通常状態のパラメーター
	RootParameters rootParameters_{};

	//ジャンプ状態のパラメーター
	JumpParameters jumpParameters_{};

	//回避状態のパラメーター
	DodgeParameters dodgeParameters_{};

	//ダッシュ用のパラメーター
	DashParameters dashParameters_{};

	//タイトルシーンなのか
	bool isInTitleScene_ = false;

	//デバッグ用のフラグ
	bool isDebug_ = false;

	//アニメーション時間
	float animationTime_ = 0.0f;

	//現在のアニメーション
	std::string currentAnimationName_ = "Idle";

	//アニメーション一覧
	std::vector<std::string> animationName_ = {
		{"Idle"},
		{"Walk1"},
		{"Walk2"},
		{"Walk3"},
		{"Walk4"},
		{"Run1"},
		{"Run2"},
		{"Run3"},
		{"Run4"},
		{"Jump1"},
		{"Jump2"},
		{"Dodge1"},
		{"Dodge2"},
		{"Dodge3"},
		{"Dodge4"},
		{"DashStart"},
		{"DashEnd"},
	};
};


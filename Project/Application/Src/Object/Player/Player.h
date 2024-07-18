#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/3D/Camera/LockOn.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "States/IPlayerState.h"

class Player : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void SetCamera(const Camera* camera) { camera_ = camera; };

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

	void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

private:
	void ChangeState(IPlayerState* state);

	void ImGui();

	void ApplyGlobalVariables();

private:
	//プレイヤーの状態
	std::unique_ptr<IPlayerState> state_ = nullptr;

	//速度
	Vector3 velocity{};

	//Quaternion
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	//Camera
	const Camera* camera_ = nullptr;

	//LockOn
	const LockOn* lockOn_ = nullptr;

	//タイトルシーンなのか
	bool isInTitleScene_ = false;

	//歩きのスティック入力の閾値
	float walkThreshold_ = 0.3f;

	//歩きの移動速度
	float walkSpeed_ = 9.0f;

	//走りのスティック入力の閾値
	float runThreshold_ = 0.6f;

	//走りの移動速度
	float runSpeed_ = 18.0f;

	//回避の速度
	float dodgeSpeed_ = 18.0f;

	//ジャスト回避の速度
	float justDodgeDistance_ = 20.0f;

	//ジャンプの初速度
	float jumpFirstSpeed_ = 45.0f;
	
	//ジャンプの重力加速度
	float gravityAcceleration_ = 2.8f;

	//ダッシュの速度
	float dashSpeed_ = 96.0f;

	//パリィの成功時間
	float parrySuccessTime_ = 0.1f;

	//デバッグ用のフラグ
	bool isDebug_ = false;

	//アニメーションの時間
	float animationTime_ = 0.0f;

	//アニメーションの名前
	std::string currentAnimationName_ = "Armature|mixamo.com|Layer0";
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
	};

	//フレンドクラスに登録
	friend class PlayerStateRoot;
	friend class PlayerStateDodge;
	friend class PlayerStateJustDodge;
	friend class PlayerStateJump;
	friend class PlayerStateGroundAttack;
	friend class PlayerStateDash;
};


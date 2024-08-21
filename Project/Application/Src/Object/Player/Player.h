#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Lockon/Lockon.h"
#include "Application/Src/Object/Player/States/IPlayerState.h"
#include <numbers>

class Player : public GameObject
{
public:
	//通常状態のパラメーター
	struct RootParameters
	{
		float walkThreshold = 0.3f; //歩きの閾値
		float walkSpeed = 9.0f;     //歩きの移動速度
		float runThreshold = 0.6f;  //走りの閾値
		float runSpeed = 18.0f;     //走りの移動速度
	};

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void ChangeState(IPlayerState* newState);

	void Move(const Vector3& velocity);

	void Rotate(const Vector3& vector);

	void LookAtEnemy();

	void SetAnimationName(const std::string& animationName);

	const bool GetIsAnimationEnd();

	void SetIsAnimationLoop(const bool isAnimationLoop);

	void SetIsAnimationStop(const bool isAnimationStop);

	const float GetAnimationTime();

	void SetAnimationTime(const float animationTime);

	const float GetAnimationSpeed();

	void SetAnimationSpeed(const float animationSpeed);

	const Vector3 GetHipLocalPosition();

	const Vector3 GetHipWorldPosition();

	const Quaternion& GetDestinationQuaternion() const { return destinationQuaternion_; };

	const Camera* GetCamera() const { return camera_; };

	void SetCamera(const Camera* camera) { camera_ = camera; };

	const Lockon* GetLockon() const { return lockon_; };

	void SetLockon(const Lockon* lockOn) { lockon_ = lockOn; };

	const RootParameters& GetRootParameters() const { return rootParameters_; };

private:
	void UpdateRotation();

	void UpdateCollider();

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
	Quaternion destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>);

	//回転の補間速度
	float quaternionInterpolationSpeed_ = 0.4f;

	//前のフレームのアニメーション後の座標
	Vector3 preAnimationHipPosition_{};

	//コライダーのオフセット値
	Vector3 colliderOffset_{};

	//カメラ
	const Camera* camera_ = nullptr;

	//ロックオン
	const Lockon* lockon_ = nullptr;

	//通常状態のパラメーター
	RootParameters rootParameters_{};

	//デバッグ用のフラグ
	bool isDebug_ = false;

	//アニメーション時間
	float animationTime_ = 0.0f;

	//現在のアニメーション
	std::string currentAnimationName_ = "Armature|mixamo.com|Layer0";

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
};


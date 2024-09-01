#pragma once
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Camera/States/ICameraState.h"
#include "Application/Src/Object/Camera/CameraPathManager.h"

class Player;

class CameraStateLockon : public ICameraState
{
public:
	void Initialize() override;

	void Update() override;

private:
	void UpdateCameraRotation();

	float CalculateRotationAngle() const;

	bool IsWithinRotationRange(const float angle) const;

	Quaternion CalculateNewRotation() const;

	void HandleDashState(const Player* player);

	void ApplyDashCameraAnimation();

	void UpdateAnimationTime();

	void UpdateCameraTransformFromKeyFrame(const CameraKeyFrame& keyFrame);

	void CheckAnimationCompletion();

	void ManageCameraStateTransition(const Player* player);

private:
	//前のフレームの方向ベクトル
	Vector3 previousDirection_{};

	//現在の方向ベクトル
	Vector3 currentDirection_{};

	//カメラパス
	CameraPath dashCameraPath_{};

	//アニメーションの時間
	float dashAnimationTime_ = 0.0f;

	//ダッシュのアニメーションがアクティブかどうか
	bool isDashCameraAnimationActive_ = false;
};


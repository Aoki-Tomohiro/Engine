#include "CameraController.h"
#include "Application/Src/Object/CameraController/States/CameraStateFollow.h"

void CameraController::Initialize()
{
	//カメラの初期化
	camera_.Initialize();
	camera_.rotationType_ = RotationType::Quaternion;

	//Stateの初期化
	ChangeState(new CameraStateFollow());
};

void CameraController::Update()
{
	//状態の更新
	state_->Update();

	//回転処理
	camera_.quaternion_ = Mathf::Normalize(Mathf::Slerp(camera_.quaternion_, destinationQuaternion_, 0.1f));

	//カメラの更新
	camera_.UpdateMatrix();
}

void CameraController::ChangeState(ICameraState* state)
{
	state->SetCameraController(this);
	state->Initialize();
	state_.reset(state);
}
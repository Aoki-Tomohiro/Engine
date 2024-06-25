#include "FollowCamera.h"
#include "Engine/Math/MathFunction.h"

void FollowCamera::Initialize()
{
	//カメラの初期化
	camera_.Initialize();
};

void FollowCamera::Update()
{
	//追従対象がいれば
	if (target_)
	{
		//追従座標の補間
		interTarget_ = Mathf::Lerp(interTarget_, target_->translation_, 0.2f);
	}

	//追従対象からのオフセット
	Vector3 offset = FollowCamera::Offset();
	//カメラ座標
	camera_.translation_ = interTarget_ + offset;

	//しきい値
	const float threshold = 0.7f;

	//回転フラグ
	bool isRotation = false;

	//回転量
	Vector3 rotation = {
		Input::GetInstance()->GetRightStickY(),
		Input::GetInstance()->GetRightStickX(),
		0.0f
	};

	//スティックの押し込みが遊び範囲を超えていたら回転フラグをtureにする
	if (Mathf::Length(rotation) > threshold) 
	{
		isRotation = true;
	}

	if (isRotation)
	{
		//回転速度
		const float kRotSpeedX = 0.02f;
		const float kRotSpeedY = 0.04f;

		//回転
		destinationAngleX_ -= rotation.x * kRotSpeedX;
		destinationAngleY_ += rotation.y * kRotSpeedY;
	}

	//回転限界角度
	const float kRotateMin = -0.14f;
	const float kRotateMax = 0.4f;
	destinationAngleX_ = std::min<float>(destinationAngleX_, kRotateMax);
	destinationAngleX_ = std::max<float>(destinationAngleX_, kRotateMin);

	//カメラのに回転を適用
	camera_.rotation_.x = Mathf::LerpShortAngle(camera_.rotation_.x, destinationAngleX_, 0.1f);
	camera_.rotation_.y = Mathf::LerpShortAngle(camera_.rotation_.y, destinationAngleY_, 0.1f);

	//カメラの更新
	camera_.UpdateMatrix();
};

Vector3 FollowCamera::Offset()
{
	//追従対象からのオフセット
	Vector3 offset = { 0.0f, 2.0f, -20.0f };
	//回転行列の合成
	Matrix4x4 rotateXMatrix = Mathf::MakeRotateXMatrix(camera_.rotation_.x);
	Matrix4x4 rotateYMatrix = Mathf::MakeRotateYMatrix(camera_.rotation_.y);
	Matrix4x4 rotateZMatrix = Mathf::MakeRotateZMatrix(camera_.rotation_.z);
	Matrix4x4 rotateMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;
	//オフセットをカメラの回転に合わせて回転させる
	offset = Mathf::TransformNormal(offset, rotateMatrix);
	return offset;
}


void FollowCamera::Reset()
{
	//追従対象がいれば
	if (target_) {
		//追従座標・角度の初期化
		interTarget_ = target_->translation_;
		camera_.rotation_.x = target_->rotation_.x;
		camera_.rotation_.y = target_->rotation_.y;
	}
	destinationAngleX_ = camera_.rotation_.x;
	destinationAngleY_ = camera_.rotation_.y;

	//追従対象からのオフセット
	Vector3 offset = Offset();
	camera_.translation_ = interTarget_ + offset;
}

void FollowCamera::SetTarget(const WorldTransform* target)
{
	target_ = target;
	Reset();
}
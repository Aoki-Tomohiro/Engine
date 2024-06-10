#include "FollowCamera.h"
#include "Application/Src/Object/LockOn/LockOn.h"
#include <numbers>

void FollowCamera::Initialize()
{
	//入力クラスのインスタンスを取得
	input_ = Input::GetInstance();

	//カメラの初期化
	camera_.Initialize();
	camera_.farClip_ = 1000.0f;
}

void FollowCamera::Update()
{
	if (clearAnimation_)
	{
		const Vector3 endOffset = { 0.0f, 2.0f, -10.0f };
		offset_ = Mathf::Lerp(offset_, endOffset, 0.1f);
		destinationAngleY_ += 0.006f;
		const float epsilon = 0.8f;
		if (Mathf::Length(offset_ - endOffset) < epsilon)
		{
			clearAnimationEnd_ = true;
		}
	}

	//追従対象がいれば
	if (target_)
	{
		//追従座標の補間
		interTarget_ = Mathf::Lerp(interTarget_, target_->translation_, 0.2f);
	}

	//追従対象からのオフセット
	Vector3 offset = Offset();

	//カメラ座標を計算
	camera_.translation_ = interTarget_ + offset;

	//カメラの回転速度を設定
	switch (cameraSensitivity_)
	{
	case 0:
		rotSpeedY_ = 0.02f;
		break;
	case 1:
		rotSpeedY_ = 0.04f;
		break;
	case 2:
		rotSpeedY_ = 0.06f;
		break;
	case 3:
		rotSpeedY_ = 0.08f;
		break;
	case 4:
		rotSpeedY_ = 0.1f;
		break;
	}

	//ロックオン中なら
	if (!clearAnimation_)
	{
		if (lockOn_ && lockOn_->ExistTarget())
		{
			//ロックオン座標
			Vector3 lockOnPosition = lockOn_->GetTargetPosition();
			//追従対象からロックオン座標へのベクトル
			Vector3 sub = lockOnPosition - target_->translation_;
			sub.y = 0.0f;

			//Y軸周り角度
			if (sub.z != 0.0) {
				destinationAngleY_ = std::asin(sub.x / std::sqrt(sub.x * sub.x + sub.z * sub.z));

				if (sub.z < 0.0) {
					destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> -destinationAngleY_ : -std::numbers::pi_v<float> -destinationAngleY_;
				}
			}
			else {
				destinationAngleY_ = (sub.x >= 0.0) ? std::numbers::pi_v<float> / 2.0f : -std::numbers::pi_v<float> / 2.0f;
			}

			destinationAngleX_ = 0.2f;
		}
		else
		{
			//旋回操作
			if (input_->IsControllerConnected())
			{

				//しきい値
				const float threshold = 0.7f;

				//回転フラグ
				bool isRotation = false;

				//回転量
				Vector3 rotation = {
					input_->GetRightStickY(),
					input_->GetRightStickX(),
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

					destinationAngleX_ -= rotation.x * kRotSpeedX;
					destinationAngleY_ += rotation.y * rotSpeedY_;
				}
			}
		}
	}

	//回転限界角度
	const float kRotateMin = -0.06f;
	const float kRotateMax = 0.4f;
	destinationAngleX_ = std::min<float>(destinationAngleX_, kRotateMax);
	destinationAngleX_ = std::max<float>(destinationAngleX_, kRotateMin);

	//回転
	camera_.rotation_.x = Mathf::LerpShortAngle(camera_.rotation_.x, destinationAngleX_, 0.1f);
	camera_.rotation_.y = Mathf::LerpShortAngle(camera_.rotation_.y, destinationAngleY_, 0.1f);

	//行列の更新
	camera_.UpdateMatrix();

	ImGui::Begin("FollowCamera");
	ImGui::DragFloat("RotSpeed", &rotSpeedY_);
	ImGui::DragFloat3("Offset", &offset_.x);
	ImGui::End();
}

void FollowCamera::SetTarget(const WorldTransform* target)
{
	target_ = target;
	Reset();
}

Vector3 FollowCamera::Offset()
{
	//追従対象からのオフセット
	Vector3 offset = offset_;
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
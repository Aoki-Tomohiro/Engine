#include "CameraController.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Camera/States/CameraStateFollow.h"
#include "Application/Src/Object/Camera/States/CameraStateClear.h"

void CameraController::Initialize()
{
	//カメラの初期化
	camera_.Initialize();
	camera_.rotationType_ = RotationType::Quaternion;

	//カメラシェイクの生成
	cameraShake_ = std::make_unique<CameraShake>();

	//状態の初期化
	ChangeState(new CameraStateFollow());
}

void CameraController::Update()
{
	//追従対象が存在する場合追従座標を補間する
	UpdateInterTargetPosition();

	//追従対象からのオフセットを計算しカメラ座標を設定
	UpdateCameraPosition();

	//編集モードやアニメーション再生の状態に応じて処理を分岐
	if (!cameraPathManager_->GetIsEditModeEnabled() && !cameraPathManager_->GetIsPlayAnimation())
	{
		//状態の更新
		state_->Update();
	}
	else
	{
		//アニメーションが再生されている場合
		if (cameraPathManager_->GetIsPlayAnimation())
		{
			UpdateAnimation();
		}
		//編集モードでない場合アニメーションの更新
		else
		{
			UpdateEditing();
		}
	}

	//補間速度の更新
	UpdateInterpolationSpeed();

	//オフセット値を更新
	UpdateCameraOffset();

	//回転処理
	UpdateCameraRotation();

	//クリアアニメーション状態に遷移するかを確認
	if (!isClearAnimationActive_)
	{
		const Enemy* enemy = GameObjectManager::GetInstance()->GetConstGameObject<Enemy>("Enemy");
		if (enemy->GetIsDead())
		{
			isClearAnimationActive_ = true;
			ChangeState(new CameraStateClear());
		}
	}

	//カメラシェイクの処理
	cameraShake_->Update();
	camera_.translation_ += cameraShake_->GetShakeOffset();

	//カメラの更新
	camera_.UpdateMatrix();
}

void CameraController::ChangeState(ICameraState* state)
{
	//新しいカメラの状態を設定
	state->SetCameraController(this);

	//新しいカメラの状態を初期化
	state->Initialize();

	//現在のカメラ状態を新しい状態に置き換える
	state_.reset(state);
}

void CameraController::StartCameraShake(const Vector3& intensity, const float duration)
{
	//カメラシェイクを開始
	cameraShake_->Start(intensity, duration);
}

void CameraController::StartInterpolationSpeedIncrease()
{
	isInterpolationSpeedIncreasing_ = true;
	interpolationSpeedTimer_ = 0.0f;
}

const Vector3 CameraController::CalculateOffset() const
{
	//追従対象からのオフセットを取得
	Vector3 offset = offset_;
	//カメラの回転に基づいてオフセットベクトルを回転させる
	offset = Mathf::RotateVector(offset, camera_.quaternion_);
	//回転を適用したオフセット値を返す
	return offset;
}

void CameraController::UpdateInterTargetPosition()
{
	//追従対象が存在する場合、追従座標を補間する
	if (target_)
	{
		Vector3 targetPosition = {
			target_->matWorld_.m[3][0],
			target_->matWorld_.m[3][1],
			target_->matWorld_.m[3][2],
		};
		interTarget_ = Mathf::Lerp(interTarget_, targetPosition, targetInterpolationSpeed_);
	}
}

void CameraController::UpdateAnimation()
{
	//ターゲットが存在する場合
	if (target_)
	{
		//現在のアニメーションのキーフレームを取得
		CameraKeyFrame cameraKeyFrame = cameraPathManager_->GetCurrentKeyFrame();

		//キーフレームから目標オフセットを設定
		destinationOffset_ = cameraKeyFrame.position;

		//キーフレームの回転情報とFOVを使用してカメラの変換を更新
		UpdateCameraTransform(cameraKeyFrame.rotation, cameraKeyFrame.fov);
	}
}

void CameraController::UpdateEditing()
{
	//ターゲットが存在する場合
	if (target_)
	{
		//編集中の新しいキーフレームを取得
		CameraKeyFrame cameraKeyFrame = cameraPathManager_->GetNewKeyFrame();

		//キーフレームから目標オフセットを設定
		destinationOffset_ = cameraKeyFrame.position;

		//キーフレームの回転情報とFOVを使用してカメラの変換を更新
		UpdateCameraTransform(cameraKeyFrame.rotation, cameraKeyFrame.fov);
	}
}

void CameraController::UpdateCameraTransform(const Quaternion& rotation, float fovDegrees)
{
	//プレイヤーの情報を取得
	const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("Player");

	//プレイヤーのクォータニオンと指定された回転を合成してカメラの目標クォータニオンを設定
	destinationQuaternion_ = player->GetDestinationQuaternion() * rotation;

	//FOVを設定
	camera_.fov_ = fovDegrees * std::numbers::pi_v<float> / 180.0f;
}

void CameraController::UpdateInterpolationSpeed()
{
	//補間速度を増加させるフラグが立っている場合
	if (isInterpolationSpeedIncreasing_)
	{
		//タイマーを経過時間で更新
		interpolationSpeedTimer_ += GameTimer::GetDeltaTime();

		//タイマーを範囲内に制限
		interpolationSpeedTimer_ = std::clamp(interpolationSpeedTimer_, 0.0f, remainingInterpolationTime_);

		//イージングパラメータを計算
		float easingParameter = interpolationSpeedTimer_ / remainingInterpolationTime_;

		//補間速度を更新
		offsetInterpolationSpeed_ = Mathf::Lerp(0.02f, 0.1f, easingParameter);
		quaternionInterpolationSpeed_ = Mathf::Lerp(0.02f, 0.2f, easingParameter);

		//タイマーが最大値に達した場合
		if (interpolationSpeedTimer_ >= remainingInterpolationTime_)
		{
			//補間速度の増加フラグを折る
			isInterpolationSpeedIncreasing_ = false;

			//タイマーをリセット
			interpolationSpeedTimer_ = 0.0f;
		}
	}
}

void CameraController::UpdateCameraPosition()
{
	//追従対象からのオフセットを計算
	Vector3 offset = CalculateOffset();

	//追従対象の位置と計算したオフセットを加算してカメラの位置を設定
	Vector3 newPosition = GetInterTarget() + offset;
	if (newPosition.y <= 0.0f)
	{
		newPosition.y = 0.0f;
	}
	SetPosition(newPosition);
}

void CameraController::UpdateCameraOffset()
{
	//現在のオフセット値を補間
	offset_ = Mathf::Lerp(offset_, destinationOffset_, offsetInterpolationSpeed_);
}

void CameraController::UpdateCameraRotation()
{
	//目標クォータニオンを正規化
	destinationQuaternion_ = Mathf::Normalize(destinationQuaternion_);

	//現在のクォータニオン補間
	camera_.quaternion_ = Mathf::Normalize(Mathf::Slerp(camera_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));
}
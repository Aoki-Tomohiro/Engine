#include "CameraStateLockOn.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Application/Src/Object/CameraController/CameraController.h"
#include "Application/Src/Object/CameraController/States/CameraStateFollow.h"
#include "Application/Src/Object/CameraController/States/CameraStateJustDodge.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void CameraStateLockOn::Initialize()
{
	//オフセット値の初期化
	cameraController_->destinationOffset_ = { 4.0f, 6.0f, -24.0f };
}

void CameraStateLockOn::Update()
{
	//プレイヤーが空中攻撃をしているときはOffset値を変える
	Player* player = GameObjectManager::GetInstance()->GetGameObject<Player>();
	if (player->GetIsAirAttack())
	{
		cameraController_->destinationOffset_ = { 4.0f, 2.0f, -24.0f };
	}
	else
	{
		//敵を取得
		Enemy* enemy = GameObjectManager::GetInstance()->GetGameObject<Enemy>();
		//敵とプレイヤーの距離が一定内の場合はOffsetのYを低くする
		float distance = Mathf::Length(player->GetComponent<TransformComponent>()->GetWorldPosition() - enemy->GetHipWorldPosition());
		const float kMaxDistance = 100.0f;
		if (distance <= kMaxDistance)
		{
			float easingParameter = 1.0f * (distance / kMaxDistance);
			Vector3 closeOffset = { 4.0f, 4.0f, -24.0f };
			Vector3 longOffset = { 4.0f, 6.0f, -24.0f };
			cameraController_->destinationOffset_ = Mathf::Lerp(closeOffset, longOffset, easingParameter);
		}
		else
		{
			cameraController_->destinationOffset_ = { 4.0f, 6.0f, -24.0f };
		}
	}

	//追従対象からのオフセットを取得してカメラ座標を計算する
	Vector3 offset = cameraController_->Offset();
	cameraController_->camera_.translation_ = cameraController_->interTarget_ + offset;

	//カメラが地面より下に行っていた場合オフセット値を変更する
	const float kMinHeight = 0.1f;
	if (cameraController_->camera_.translation_.y <= kMinHeight)
	{
		//オフセット値をカメラがめり込んだ量だけ引く
		cameraController_->destinationOffset_.z -= cameraController_->camera_.translation_.y;
		cameraController_->offset_.z -= cameraController_->camera_.translation_.y;

		//オフセット値を再計算
		offset = cameraController_->Offset();
		cameraController_->camera_.translation_ = cameraController_->interTarget_ + offset;
	}

	//ロックオン対象の座標を取得
	Vector3 lockOnPosition = cameraController_->lockOn_->GetTargetPosition();

    //差分ベクトルを計算
	Vector3 sub = Mathf::Normalize(lockOnPosition - cameraController_->camera_.translation_);

    //Quaternionの作成
	cameraController_->destinationQuaternion_ = Mathf::LookRotation(sub, { 0.0f,1.0f,0.0f });

	//追従カメラに遷移
	if (!cameraController_->lockOn_->ExistTarget())
	{
		cameraController_->ChangeState(new CameraStateFollow());
	}
	//ジャスト回避のカメラに遷移
	else if (player->GetIsJustDodgeSuccess())
	{
		cameraController_->ChangeState(new CameraStateJustDodge());
	}
}
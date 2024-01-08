#include "BossStateLaserAttack.h"
#include "Project/Object/Boss/Boss.h"
#include "Engine/Components/CollisionConfig.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Base/ImGuiManager.h"

void BossStateLaserAttack::Initialize(Boss* pBoss)
{
	worldTransform_ = pBoss->GetWorldTransform();
	destinationQuaternion_ = Quaternion{ 0.0f,0.0f,0.0f,1.0f };

	//警告モデルの作成
	waringModel_ = ModelManager::CreateFromOBJ("Warning", Opaque);
	waringModel_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	waringModel_->Update();

	//警告用のワールドトランスフォームの初期化
	warningWorldTransform_.Initialize();
	warningWorldTransform_.translation_ = targetPosition_;
	warningWorldTransform_.translation_.y = 0.11f;
	warningWorldTransform_.quaternion_ = destinationQuaternion_;
	warningWorldTransform_.scale_ = { 2.0f,2.0f,70.0f };
}

void BossStateLaserAttack::Update(Boss* pBoss)
{
	//移動処理
	worldTransform_.translation_ = Mathf::Lerp(worldTransform_.translation_, targetPosition_, 0.1f);

	const float epsilon = 0.001f;
	Vector3 abs = {
		std::abs(worldTransform_.translation_.x - targetPosition_.x),
		std::abs(worldTransform_.translation_.y - targetPosition_.y),
		std::abs(worldTransform_.translation_.z - targetPosition_.z),
	};
	//目標位置についたらレーザーをためる
	if (abs.x < epsilon && abs.y < epsilon && abs.z < epsilon)
	{
		isCharge_ = true;
		//ため処理
		if (!isAttack_)
		{
			if (++chargeTimer_ > kChargeTime)
			{
				//攻撃フラグをtrueにする
				isAttack_ = true;
				isCharge_ = false;

				//レーザーを追加
				Laser* newLaser = new Laser();
				newLaser->Initialize();
				pBoss->AddLaser(newLaser);
			}
		}

		//攻撃処理
		if (isAttack_)
		{
			if (!isRecovery_)
			{
				//回転処理
				Quaternion rotateQuaternion = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, 0.02f);
				destinationQuaternion_ *= rotateQuaternion;

				if (++attackTimer_ > Laser::kLifeTime)
				{
					isRecovery_ = true;
				}
			}
		}

		//硬直処理
		if (isRecovery_)
		{
			if (++recoveryTimer_ > kRecoveryTime)
			{
				IBossState* newState = new BossStateNormal();
				newState->Initialize(pBoss);
				pBoss->ChangeState(newState);
				return;
			}
		}
	}

	//回転処理
	worldTransform_.quaternion_ = Mathf::Slerp(worldTransform_.quaternion_, destinationQuaternion_, 0.4f);

	//移動限界座標
	const float kMoveLimitX = 47;
	const float kMoveLimitZ = 47;
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.z = max(worldTransform_.translation_.z, -kMoveLimitZ);
	worldTransform_.translation_.z = min(worldTransform_.translation_.z, +kMoveLimitZ);

	//警告用のワールドトランスフォームの更新
	warningWorldTransform_.UpdateMatrixFromQuaternion();

	ImGui::Begin("LaserAttack");
	ImGui::End();
}

void BossStateLaserAttack::Draw(Boss* pBoss, const Camera& camera)
{
	if (isCharge_ && !isAttack_)
	{
		waringModel_->Draw(warningWorldTransform_, camera);
	}
}

void BossStateLaserAttack::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() == kCollisionAttributeWeapon)
	{

	}
}
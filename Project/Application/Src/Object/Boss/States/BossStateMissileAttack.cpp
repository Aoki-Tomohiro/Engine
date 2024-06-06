#include "BossStateMissileAttack.h"
#include "Application/Src/Object/Boss/Boss.h"

void BossStateMissileAttack::Initialize(Boss* pBoss)
{
	worldTransform_ = pBoss->GetWorldTransform();
	destinationQuaternion_ = worldTransform_.quaternion_;
	audioHandle_ = Audio::GetInstance()->LoadAudioFile("Application/Resources/Sounds/Fire.mp3");
}

void BossStateMissileAttack::Update(Boss* pBoss)
{
	//待機処理
	if (!isAttack_)
	{
		if (++waitTimer_ > kWaitTime)
		{
			isAttack_ = true;
		}
	}

	//攻撃処理
	if (isAttack_ && !isRecovery_)
	{
		if (++fireTimer_ > kFireInterval)
		{
			fireTimer_ = 0;
			fireCount_++;
			Vector3 velocity[4];
			velocity[0] = { 0.2f,0.2f,0.0f };
			velocity[0] = Mathf::TransformNormal(velocity[0], worldTransform_.matWorld_);
			velocity[1] = { -0.2f,0.2f,0.0f };
			velocity[1] = Mathf::TransformNormal(velocity[1], worldTransform_.matWorld_);

			Vector3 translation = pBoss->GetCollider()->GetWorldPosition();
			Missile* newMissile1 = new Missile();
			newMissile1->Initialize(translation, velocity[0]);
			newMissile1->SetCollider(new Collider());
			newMissile1->GetCollider()->SetCollisionAttribute(kCollisionAttributeMissile);
			newMissile1->GetCollider()->SetCollisionMask(kCollisionMaskMissile);
			newMissile1->GetCollider()->SetCollisionPrimitive(kCollisionPrimitiveAABB);
			pBoss->AddMissile(newMissile1);

			Missile* newMissile2 = new Missile();
			newMissile2->Initialize(translation, velocity[1]);
			newMissile2->SetCollider(new Collider());
			newMissile2->GetCollider()->SetCollisionAttribute(kCollisionAttributeMissile);
			newMissile2->GetCollider()->SetCollisionMask(kCollisionMaskMissile);
			newMissile2->GetCollider()->SetCollisionPrimitive(kCollisionPrimitiveAABB);
			pBoss->AddMissile(newMissile2);

			//音声再生
			Audio::GetInstance()->PlayAudio(audioHandle_, false, 0.5f);
		}

		if (fireCount_ >= 3)
		{
			isRecovery_ = true;
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
		}
	}

	//移動限界座標
	const float kMoveLimitX = 47;
	const float kMoveLimitZ = 47;
	worldTransform_.translation_.x = std::max<float>(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = std::min<float>(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.z = std::max<float>(worldTransform_.translation_.z, -kMoveLimitZ);
	worldTransform_.translation_.z = std::min<float>(worldTransform_.translation_.z, +kMoveLimitZ);
}

void BossStateMissileAttack::Draw(Boss* pBoss, const Camera& camera)
{

}

void BossStateMissileAttack::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() == kCollisionAttributeWeapon)
	{

	}
}
#include "MagicProjectile.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Utilities/GameTimer.h"

int32_t MagicProjectile::counter = 0;

void MagicProjectile::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();

	//カウンターを増やす
	counter++;

	//自身のIDの初期化
	id_ = counter;

	//パーティクルの初期化
	particleSystem_ = ParticleManager::Create("MagicProjectile");

	//エミッターの作成
	ParticleEmitter* emitter = EmitterBuilder()
		.SetColor({ 1.0f, 0.4f, 0.4f, 1.0f }, { 1.0f, 0.4f, 0.4f, 1.0f })
		.SetCount(20)
		.SetEmitterLifeTime(10.0f)
		.SetEmitterName("MagicProjectileMove" + std::to_string(id_))
		.SetFrequency(0.01f)
		.SetLifeTime(0.3f, 0.6f)
		.SetRadius(0.1f)
		.SetScale({ 0.2f,0.2f,0.2f }, { 0.2f,0.2f,0.2f })
		.SetTranslation({ 0.0f,0.0f,0.0f })
		.SetVelocity({ -1.0f,-1.0f,-1.0f }, { 1.0f,1.0f,-1.0f })
		.Build();
	particleSystem_->AddParticleEmitter(emitter);
}

void MagicProjectile::Update()
{
	//移動処理
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.translation_ += velocity_ * GameTimer::GetDeltaTime();

	//エミッターを移動させる
	if (ParticleEmitter* emitter = particleSystem_->GetParticleEmitter("MagicProjectileMove" + std::to_string(id_)))
	{		
		//エミッターの座標を更新する
		emitter->SetTranslate(transformComponent->worldTransform_.translation_);

		//速さ
		float minSpeed = 0.0f;
		float maxSpeed = 0.0f;

		//種類に応じて速度を変える
		switch (magicType_)
		{
		case MagicType::kNormal:
			minSpeed = 0.2f;
			maxSpeed = 0.3f;
			emitter->SetCount(20);
			break;
		case MagicType::kEnhanced:
			minSpeed = 0.4f;
			maxSpeed = 0.5f;
			emitter->SetCount(30);
			break;
		case MagicType::kCharged:
			minSpeed = 0.6f;
			maxSpeed = 0.7f;
			emitter->SetCount(40);
			break;
		}

		//最低速度を計算
		Vector3 minVelocity{ -1.0f,-1.0f,-1.0f };
		minVelocity = Mathf::RotateVector(minVelocity, transformComponent->worldTransform_.quaternion_) * minSpeed;
		//最高速度を計算
		Vector3 maxVelocity{ 1.0f,1.0f,-1.0f };
		maxVelocity = Mathf::RotateVector(maxVelocity, transformComponent->worldTransform_.quaternion_) * maxSpeed;

		//パーティクルの初期速度を更新
		emitter->SetVelocityMin(minVelocity);
		emitter->SetVelocityMax(maxVelocity);
	}

	//フィールド外に出たら破壊フラグを立てる
	const float kFieldMinX = -80;
	const float kFieldMaxX = 80;
	const float kFieldMinZ = -80;
	const float kFieldMaxZ = 80;
	if (transformComponent->worldTransform_.translation_.x < kFieldMinX || transformComponent->worldTransform_.translation_.x > kFieldMaxX)
	{
		DeleteMagicProjectile();
	}
	else if (transformComponent->worldTransform_.translation_.z < kFieldMinZ || transformComponent->worldTransform_.translation_.z > kFieldMaxZ)
	{
		DeleteMagicProjectile();
	}

	//基底クラスの更新
	GameObject::Update();
}

void MagicProjectile::Draw(const Camera& camera)
{
	//基底クラスの描画
	GameObject::Draw(camera);
}

void MagicProjectile::DrawUI()
{
}

void MagicProjectile::OnCollision(GameObject* gameObject)
{
	//ヒットフラグを立てる
	isHit_ = true;

	DeleteMagicProjectile();
}

void MagicProjectile::OnCollisionEnter(GameObject* gameObject)
{
}

void MagicProjectile::OnCollisionExit(GameObject* gameObject)
{
}

void MagicProjectile::DeleteMagicProjectile()
{
	//カウンターを減らす
	counter--;

	//破壊フラグを立てる
	SetIsDestroy(true);

	//エミッターを削除
	if (ParticleEmitter* emitter = particleSystem_->GetParticleEmitter("MagicProjectileMove" + std::to_string(id_)))
	{
		emitter->SetIsDead(true);
	}
}
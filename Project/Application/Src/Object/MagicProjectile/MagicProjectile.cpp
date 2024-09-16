#include "MagicProjectile.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Collision/SphereCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"

//初期化用カウンタの定義
int32_t MagicProjectile::counter = 0;

void MagicProjectile::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();

	//トランスフォームの初期化
	InitializeTransform();

	//モデルの初期化
	InitializeModel();

	//コライダーの初期化
	InitializeCollider();

	//カウンターを増やし、IDを設定
	id_ = ++counter;

	//パーティクルシステムの初期化
	InitializeParticleSystem();
}

void MagicProjectile::Update()
{
	//移動処理
	UpdateMovement();

	//エミッターの座標を更新
	UpdateEmitterPosition();

	//パーティクルの初期速度を更新
	UpdateParticleEmitterVelocity();

	//フィールド外に出たら破壊
	CheckOutOfBounds();

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
	//魔法を削除
	DeleteMagicProjectile();
}

void MagicProjectile::OnCollisionEnter(GameObject* gameObject)
{
}

void MagicProjectile::OnCollisionExit(GameObject* gameObject)
{
}

void MagicProjectile::InitializeTransform()
{
	//トランスフォームコンポーネントを取得し、スケールを設定
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.scale_ = { 0.2f, 0.2f, 0.2f };
}

void MagicProjectile::InitializeModel()
{
	//モデルコンポーネントを取得し、シャドウのキャストを無効化
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	Model* model = modelComponent->GetModel();
	model->SetCastShadows(false);

	//マテリアルの初期化
	for (size_t i = 0; i < model->GetNumMaterials(); ++i)
	{
		Material* material = model->GetMaterial(i);
		material->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); //色の設定
		material->SetEnableLighting(false); //ライティングをしない
		material->SetEnvironmentCoefficient(0.0f); //環境係数の設定
	}
}

void MagicProjectile::InitializeCollider()
{
	//コライダーの初期化
	SphereCollider* sphereCollider = GetComponent<SphereCollider>();
	sphereCollider->SetCollisionEnabled(true);
	sphereCollider->SetCollisionAttribute(CollisionAttributeManager::GetInstance()->GetAttribute("PlayerWeapon"));
	sphereCollider->SetCollisionMask(CollisionAttributeManager::GetInstance()->GetMask("PlayerWeapon"));
}

void MagicProjectile::InitializeParticleSystem()
{
	//パーティクルシステムの初期化とエミッターの作成
	particleSystem_ = ParticleManager::Create("MagicProjectile");
	emitter_ = EmitterBuilder()
		.SetColor({ 1.0f, 0.4f, 0.4f, 1.0f }, { 1.0f, 0.4f, 0.4f, 1.0f })
		.SetCount(20)
		.SetEmitterLifeTime(10.0f)
		.SetEmitterName("MoveMagicProjectile" + std::to_string(id_))
		.SetFrequency(0.01f)
		.SetLifeTime(0.3f, 0.6f)
		.SetRadius(0.4f)
		.SetScale({ 0.4f,0.4f,0.4f }, { 0.4f,0.4f,0.4f })
		.SetTranslation({ 0.0f,0.0f,0.0f })
		.SetVelocity({ -1.0f,-1.0f,-1.0f }, { 1.0f,1.0f,-1.0f })
		.Build();
	particleSystem_->AddParticleEmitter(emitter_);
}

void MagicProjectile::UpdateMovement()
{
	//移動処理
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.translation_ += velocity_ * GameTimer::GetDeltaTime();
}

void MagicProjectile::UpdateEmitterPosition()
{
	//エミッターの位置を魔法の位置に更新
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	emitter_->SetTranslate(transformComponent->worldTransform_.translation_);
}

void MagicProjectile::UpdateParticleEmitterVelocity()
{
	//パーティクルエミッターの速度範囲を更新
	float minSpeed = 0.0f;
	float maxSpeed = 0.0f;

	//魔法の種類に応じて速度とパーティクル数を設定
	switch (magicType_)
	{
	case MagicType::kNormal:
		minSpeed = 0.2f;
		maxSpeed = 0.3f;
		emitter_->SetCount(20);
		break;
	case MagicType::kCharged:
		minSpeed = 0.6f;
		maxSpeed = 0.7f;
		emitter_->SetCount(40);
		break;
	}

	//パーティクルの最小速度と最大速度を設定
	Vector3 minVelocity{ -1.0f, -1.0f, -1.0f };
	minVelocity = Mathf::RotateVector(minVelocity, GetComponent<TransformComponent>()->worldTransform_.quaternion_) * minSpeed;
	Vector3 maxVelocity{ 1.0f, 1.0f, -1.0f };
	maxVelocity = Mathf::RotateVector(maxVelocity, GetComponent<TransformComponent>()->worldTransform_.quaternion_) * maxSpeed;
	emitter_->SetVelocityMin(minVelocity);
	emitter_->SetVelocityMax(maxVelocity);
}

void MagicProjectile::CheckOutOfBounds()
{
	//フィールド外に出たか確認し、出ていたら削除
	const float moveLimit = 300.0f;
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	float distance = Mathf::Length(transformComponent->worldTransform_.translation_);
	if (distance > moveLimit)
	{
		DeleteMagicProjectile();
	}
}

void MagicProjectile::CreateDestoryParticles()
{
	//トランスフォームを取得
	TransformComponent* transformComponent = GetComponent<TransformComponent>();

	//ヒットエフェクト
	ParticleEmitter* hitNewEmitter = EmitterBuilder()
		.SetEmitterName("Hit")
		.SetEmitterLifeTime(1.0f)
		.SetTranslation(transformComponent->worldTransform_.translation_)
		.SetCount(200)
		.SetColor({ 1.0f, 0.2f, 0.2f, 1.0f }, { 1.0f, 0.2f, 0.2f, 1.0f })
		.SetFrequency(2.0f)
		.SetLifeTime(0.2f, 0.4f)
		.SetRadius(0.0f)
		.SetScale({ 0.2f,0.2f,0.2f }, { 0.3f,0.3f,0.3f })
		.SetVelocity({ -0.4f,-0.4f,-0.4f }, { 0.4f,0.4f,0.4f })
		.Build();
	particleSystem_->AddParticleEmitter(hitNewEmitter);
}

void MagicProjectile::DeleteMagicProjectile()
{
	//カウンターを減らす
	counter--;

	//破壊フラグを立てる
	SetIsDestroy(true);

	//パーティクルを出す
	CreateDestoryParticles();

	//エミッターを削除
	emitter_->SetIsDead(true);
}
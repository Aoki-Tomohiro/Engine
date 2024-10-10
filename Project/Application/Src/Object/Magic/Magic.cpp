#include "Magic.h"

//初期化用カウンタの定義
int32_t Magic::counter = 0;

void Magic::Initialize()
{
	//カウンターを増やし、IDを設定
	id_ = ++counter;

	//トランスフォームの初期化
	InitializeTransform();

	//モデルの初期化
	InitializeModel();

	//コライダーの初期化
	InitializeCollider();

	//パーティクルシステムの初期化
	InitializeParticleSystem();

	//基底クラスの初期化
	GameObject::Initialize();
}

void Magic::Update()
{
	//移動処理
	transformComponent_->worldTransform_.translation_ += velocity_ * GameTimer::GetDeltaTime();

	//移動パーティクルの更新
	UpdateMoveEmitter();

	//フィールド外に出たら破壊
	CheckOutOfBounds();

	//基底クラスの更新
	GameObject::Update();
}

void Magic::OnCollision(GameObject* gameObject)
{
	//魔法を削除
	DeleteMagic();
}

void Magic::InitializeTransform()
{
	//トランスフォームの初期化
	transformComponent_ = GetComponent<TransformComponent>();
	transformComponent_->worldTransform_.scale_ = scale_;
}

void Magic::InitializeModel()
{
	//モデルコンポーネントを追加
	modelComponent_ = AddComponent<ModelComponent>();

	//モデルを生成
	Model* model = ModelManager::CreateFromModelFile("Sphere", Opaque);

	//モデルコンポーネントにモデルを設定
	modelComponent_->SetModel(model);

	//マテリアルの初期化
	for (size_t i = 0; i < model->GetNumMaterials(); ++i)
	{
		Material* material = model->GetMaterial(i);
		material->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); //色の設定
		material->SetEnableLighting(false); //ライティングをしない
		material->SetEnvironmentCoefficient(0.0f); //環境係数の設定
	}
}

void Magic::InitializeCollider()
{
	//コライダーの初期化
	SphereCollider* sphereCollider = AddComponent<SphereCollider>();
	sphereCollider->SetRadius(scale_.x);
	sphereCollider->SetCollisionAttribute(CollisionAttributeManager::GetInstance()->GetAttribute("PlayerWeapon"));
	sphereCollider->SetCollisionMask(CollisionAttributeManager::GetInstance()->GetMask("PlayerWeapon"));
}

void Magic::InitializeParticleSystem()
{
	//パーティクルの生成
	particleSystem_ = ParticleManager::Create("MagicProjectile");

	//移動のパーティクルを生成
	CreateMoveEmitter();

	//パーティクルシステムにエミッターを追加
	particleSystem_->AddParticleEmitter(emitter_);
}

void Magic::CreateMoveEmitter()
{
	//パーティクルエミッターの速度範囲を更新
	float minSpeed = magicType_ == MagicType::kNormal ? normalMagicMinSpeed_ : chargeMagicMinSpeed_;
	float maxSpeed = magicType_ == MagicType::kNormal ? normalMagicMaxSpeed_ : chargeMagicMaxSpeed_;
	int32_t emitCount = magicType_ == MagicType::kNormal ? normalMagicEmitCount_ : chargeMagicEmitCount_;

	//パーティクルの最小速度と最大速度を設定
	Vector3 minVelocity = Mathf::RotateVector({ -1.0f, -1.0f, -1.0f }, transformComponent_->worldTransform_.quaternion_) * minSpeed;
	Vector3 maxVelocity = Mathf::RotateVector({ 1.0f, 1.0f, -1.0f }, transformComponent_->worldTransform_.quaternion_) * maxSpeed;

	//パーティクルエミッターを生成
	emitter_ = EmitterBuilder().SetColor({ 1.0f, 0.4f, 0.4f, 1.0f }, { 1.0f, 0.4f, 0.4f, 1.0f }).SetCount(emitCount)
		.SetEmitterLifeTime(10.0f).SetEmitterName("MoveMagicProjectile" + std::to_string(id_)).SetFrequency(0.01f)
		.SetLifeTime(0.3f, 0.6f).SetRadius(0.4f).SetScale({ 0.4f,0.4f,0.4f }, { 0.4f,0.4f,0.4f }).SetTranslation({ 0.0f,0.0f,0.0f })
		.SetVelocity(minVelocity, maxVelocity).Build();
}

void Magic::UpdateMoveEmitter()
{
	//エミッターが存在しなければ生成する
	if (!particleSystem_->GetParticleEmitter("MoveMagicProjectile" + std::to_string(id_)))
	{
		CreateMoveEmitter();
	}

	//エミッターの座標を更新
	emitter_->SetTranslate(transformComponent_->worldTransform_.translation_);
}

void Magic::CheckOutOfBounds()
{
	//移動制限
	const float moveLimit = 300.0f;

	//原点からの距離を計算
	float distance = Mathf::Length(transformComponent_->worldTransform_.translation_);

	//距離が移動制限を超えていたら破壊する
	if (distance > moveLimit || transformComponent_->worldTransform_.translation_.y < 0.0f)
	{
		DeleteMagic();
	}
}

void Magic::DeleteMagic()
{
	//破壊フラグを立てる
	SetIsDestroy(true);

	//破壊パーティクルを出す
	CreateDestoryParticles();

	//エミッターを削除
	emitter_->SetCount(0);

	//カウンターを減らす
	--counter;
}

void Magic::CreateDestoryParticles()
{
	//エミッターを生成
	ParticleEmitter* newEmitter = EmitterBuilder().SetEmitterName("Hit" + std::to_string(id_)).SetEmitterLifeTime(0.01f)
		.SetTranslation(transformComponent_->worldTransform_.translation_).SetCount(200).SetColor({ 1.0f, 0.2f, 0.2f, 1.0f }, { 1.0f, 0.2f, 0.2f, 1.0f })
		.SetFrequency(2.0f).SetLifeTime(0.2f, 0.4f).SetRadius(0.0f).SetScale({ 0.2f,0.2f,0.2f }, { 0.3f,0.3f,0.3f })
		.SetVelocity({ -0.4f,-0.4f,-0.4f }, { 0.4f,0.4f,0.4f }).Build();

	//パーティクルシステムにエミッターを追加
	particleSystem_->AddParticleEmitter(newEmitter);
}
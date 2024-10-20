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

	//エミッターを生成
	CreateMoveEmitter();

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

void Magic::CreateMoveEmitter()
{
	//パーティクルエフェクトの名前を設定
	std::string particleEffectName = magicType_ == MagicType::kNormal ? "MagicTrail" : "ChargeMagicTrail";

	//移動パーティクルを生成
	particleEffectManager_->CreateParticles(particleEffectName, { 0.0f,0.0f,0.0f }, transformComponent_->worldTransform_.quaternion_);

	//エミッターを取得して名前を変更
	emitter_ = particleEffectManager_->GetEmitter("Normal", "MagicTrail");
	emitter_->SetName("MagicTrail" + std::to_string(id_));
}

void Magic::UpdateMoveEmitter()
{
	//エミッターが存在しなければ生成する
	if (!particleEffectManager_->GetEmitter("Normal", "MagicTrail" + std::to_string(id_)))
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
	particleEffectManager_->CreateParticles("MagicDissipation", transformComponent_->worldTransform_.translation_, transformComponent_->worldTransform_.quaternion_);

	//エミッターを削除
	emitter_->SetIsDead(true);

	//カウンターを減らす
	--counter;
}
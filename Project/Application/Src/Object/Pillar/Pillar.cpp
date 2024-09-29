#include "Pillar.h"

void Pillar::Initialize()
{
	//トランスフォームの初期化
	InitializeTransform();

	//モデルの初期化
	InitializeModel();

	//コライダーの初期化
	InitializeCollider();

	//基底クラスの初期化
	GameObject::Initialize();
}

void Pillar::Update()
{
	//状態ごとの更新
	UpdatePillarState();

	//コライダーの更新
	UpdateCollider();

	//基底クラスの更新
	GameObject::Update();
}

void Pillar::Draw(const Camera& camera)
{
	//基底クラスの描画
	GameObject::Draw(camera);
}

void Pillar::DrawUI()
{
}

void Pillar::OnCollision(GameObject* gameObject)
{
}

void Pillar::OnCollisionEnter(GameObject* gameObject)
{
}

void Pillar::OnCollisionExit(GameObject* gameObject)
{
}

void Pillar::Reset()
{
	//状態の初期化
	pillarState_ = PillarState::kInactive;

	//透明度を初期化
	alpha_ = 1.0f;

	//モデルの色を初期化
	Vector4 currentColor = model_->GetModel()->GetMaterial(1)->GetColor();
	Vector4 nextColor = { currentColor.x, currentColor.y, currentColor.z, alpha_ };
	for (int32_t i = 0; i < model_->GetModel()->GetNumMaterials(); ++i)
	{
		model_->GetModel()->GetMaterial(i)->SetColor(nextColor);
	}
}

void Pillar::InitializeTransform()
{
	//トランスフォームの初期化
	transform_ = GetComponent<TransformComponent>();
	transform_->worldTransform_.rotationType_ = RotationType::Quaternion;
}

void Pillar::InitializeModel()
{
	//モデルの追加
	model_ = AddComponent<ModelComponent>();
	Model* model = ModelManager::CreateFromModelFile("Spear", Transparent);
	model->SetCastShadows(false);
	model_->SetModel(model);
}

void Pillar::InitializeCollider()
{
	//コライダーの追加
	collider_ = AddComponent<OBBCollider>();
	collider_->SetCollisionAttribute(CollisionAttributeManager::GetInstance()->GetAttribute("Pillar"));
	collider_->SetCollisionMask(CollisionAttributeManager::GetInstance()->GetMask("Pillar"));
}

void Pillar::UpdatePillarState()
{
	//状態ごとの処理
	switch (pillarState_)
	{
	case PillarState::kInactive:
		InactiveUpdate();
		break;
	case PillarState::kSpawning:
		SpawnUpdate();
		break;
	case PillarState::kDespawning:
		DespawnUpdate();
		break;
	}
}

void Pillar::InactiveUpdate()
{
	//タイマーを進める
	inactiveTimer_ += GameTimer::GetDeltaTime();

	//タイマーが一定値を超えていた場合
	if (inactiveTimer_ > inactiveDuration_)
	{
		//出現中の状態にする
		pillarState_ = PillarState::kSpawning;

		//コライダーを有効にする
		collider_->SetCollisionEnabled(true);

		//タイマーのリセット
		inactiveTimer_ = 0.0f;
	}
}

void Pillar::SpawnUpdate()
{
	//タイマーを進める
	spawnTimer_ += GameTimer::GetDeltaTime();

	//イージング係数を計算
	float easingParameter = std::min<float>(1.0f, spawnTimer_ / spawnDuration_);

	//イージングさせる
	transform_->worldTransform_.translation_ = preSpawnPosition_ + (spawnedPosition_ - preSpawnPosition_) * Mathf::EaseInCubic(easingParameter);

	//タイマーが一定値を超えていた場合
	if (spawnTimer_ > spawnDuration_)
	{
		//出現終了状態にする
		pillarState_ = PillarState::kDespawning;

		//コライダーを無効にする
		collider_->SetCollisionEnabled(false);

		//タイマーのリセット
		spawnTimer_ = 0.0f;
	}
}

void Pillar::DespawnUpdate()
{
	//タイマーを進める
	despawnTimer_ += GameTimer::GetDeltaTime();

	//徐々に透明にする
	alpha_ = std::clamp(1.0f - (despawnTimer_ / despawnDuration_), 0.0f, 1.0f);

	//色を設定
	Vector4 currentColor = model_->GetModel()->GetMaterial(1)->GetColor();
	Vector4 nextColor = { currentColor.x, currentColor.y, currentColor.z, alpha_ };
	for (int32_t i = 0; i < model_->GetModel()->GetNumMaterials(); ++i)
	{
		model_->GetModel()->GetMaterial(i)->SetColor(nextColor);
	}

	//タイマーが一定値を超えていたらタイマーをリセットして破壊する
	if (despawnTimer_ > despawnDuration_)
	{
		SetIsDestroy(true);
		despawnTimer_ = 0.0f;
	}
}

void Pillar::UpdateCollider()
{
	//コライダーの向きとサイズを更新
	collider_->SetOrientations(
		{ transform_->worldTransform_.matWorld_.m[0][0], transform_->worldTransform_.matWorld_.m[0][1], transform_->worldTransform_.matWorld_.m[0][2] },
		{ transform_->worldTransform_.matWorld_.m[1][0], transform_->worldTransform_.matWorld_.m[1][1], transform_->worldTransform_.matWorld_.m[1][2] },
		{ transform_->worldTransform_.matWorld_.m[2][0], transform_->worldTransform_.matWorld_.m[2][1], transform_->worldTransform_.matWorld_.m[2][2] }
	);
	collider_->SetSize(colliderSize_);
}
#include "Laser.h"

void Laser::Initialize()
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

void Laser::Update()
{
	//レーザーのフェードアウト処理
	UpdateLaserFading();

	//コライダーの更新
	UpdateCollider();

	//基底クラスの更新
	GameObject::Update();
}

void Laser::InitializeTransform()
{
	//トランスフォームの初期化
	transform_ = GetComponent<TransformComponent>();
	transform_->worldTransform_.rotationType_ = RotationType::Quaternion;
}

void Laser::InitializeModel()
{
	//モデルコンポーネントを追加
	modelComponent_ = AddComponent<ModelComponent>();

	//モデルを生成
	Model* model = ModelManager::CreateFromModelFile("Laser", Opaque);

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

void Laser::InitializeCollider()
{
	//コライダーの追加
	collider_ = AddComponent<OBBCollider>();
	collider_->SetCollisionAttribute(CollisionAttributeManager::GetInstance()->GetAttribute("Laser"));
	collider_->SetCollisionMask(CollisionAttributeManager::GetInstance()->GetMask("Laser"));
}

void Laser::UpdateLaserFading()
{
	//レーザーが消えかけていない場合
	if (!isLaserFading_)
	{
		//イージングのタイマーを進める
		easingParameter_ += easingSpeed_ * GameTimer::GetDeltaTime();
		easingParameter_ = std::clamp(easingParameter_, 0.0f, 1.0f);

		//スケールを徐々に大きくする
		Vector3 startScale = { 0.0f, 0.0f, targetScale_.z };
		transform_->worldTransform_.scale_ = startScale + (targetScale_ - startScale) * Mathf::EaseOutExpo(easingParameter_);

		//レーザーのフェードアウト時間を更新
		laserFadingTimer_ += GameTimer::GetDeltaTime();

		//フェードアウト時間が生存時間を超えたら徐々にレーザーを消す
		if (laserFadingTimer_ > lifeTime_)
		{
			//レーザーが消えかけている状態に切り替え
			isLaserFading_ = true;
			easingParameter_ = 0.0f;
		}
	}
	else
	{
		//レーザーの消失処理
		FadeOutLaser();
	}
}

void Laser::FadeOutLaser()
{
	//イージングのタイマーを進める
	easingParameter_ += easingSpeed_ * GameTimer::GetDeltaTime();

	//スケールを徐々に小さくする
	Vector3 startScale = { 0.0f, 0.0f, targetScale_.z };
	transform_->worldTransform_.scale_ = targetScale_ + (startScale - targetScale_) * Mathf::EaseInCubic(easingParameter_);

	//イージングが終了したらオブジェクトを削除
	if (easingParameter_ >= 1.0f)
	{
		SetIsDestroy(true);
	}
}

void Laser::UpdateCollider()
{
	//コライダーの向きとサイズを更新
	collider_->SetOrientations(
		{ transform_->worldTransform_.matWorld_.m[0][0], transform_->worldTransform_.matWorld_.m[0][1], transform_->worldTransform_.matWorld_.m[0][2] },
		{ transform_->worldTransform_.matWorld_.m[1][0], transform_->worldTransform_.matWorld_.m[1][1], transform_->worldTransform_.matWorld_.m[1][2] },
		{ transform_->worldTransform_.matWorld_.m[2][0], transform_->worldTransform_.matWorld_.m[2][1], transform_->worldTransform_.matWorld_.m[2][2] }
	);
	collider_->SetSize(transform_->worldTransform_.scale_);
}
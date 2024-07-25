#include "Missile.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"

void Missile::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();
}

void Missile::Update()
{
	//プレイヤーのTransformを取得
	TransformComponent* playerTransformComponent = GameObjectManager::GetInstance()->GetGameObject<Player>()->GetComponent<TransformComponent>();

	//目標への差分ベクトルを計算
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	Vector3 sub = playerTransformComponent->GetWorldPosition() - transformComponent->GetWorldPosition();

	//追尾用の媒介変数の更新
	if (slerpProgress_ <= 1.0f)
	{
		slerpProgress_ += GameTimer::GetDeltaTime() * missileParameters_.interpolationRate;
	}
	float slerpFactor = 0.0f + (missileParameters_.maxSlerpFactor - 0.0f) * Mathf::EaseOutExpo(slerpProgress_);

	//追尾が終了していないとき
	if (!isTrackingComplete_)
	{
		//追尾タイマーを進める
		trackingTimer_ += GameTimer::GetDeltaTime();

		//追尾タイマーが一定値を超えていたら
		if (trackingTimer_ >= missileParameters_.trackingTimeLimit)
		{
			//追尾終了のフラグを立てる
			isTrackingComplete_ = true;
		}
	}

	//追尾が終了していなかったら
	if (!isTrackingComplete_)
	{
		//正規化
		sub = Mathf::Normalize(sub);
		velocity_ = Mathf::Normalize(velocity_);
		//速度ベクトルを球面線形補間する
		velocity_ = Mathf::Slerp(velocity_, sub, slerpFactor);
		//速度を掛ける
		velocity_ *= missileParameters_.speedMultiplier;
	}

	//移動処理
	const float fieldMin = -80.0f;
	const float fieldMax = 80.0f;
	transformComponent->worldTransform_.translation_ += velocity_ * GameTimer::GetDeltaTime();

	//フィールド外に出たら破壊フラグを立てる
	if (transformComponent->worldTransform_.translation_.x < fieldMin || transformComponent->worldTransform_.translation_.x > fieldMax
		|| transformComponent->worldTransform_.translation_.z < fieldMin || transformComponent->worldTransform_.translation_.z > fieldMax
		|| transformComponent->worldTransform_.translation_.y <= 0.0f)
	{
		//破壊フラグを立てる
		SetIsDestroy(true);
	}


	//基底クラスの更新
	GameObject::Update();
}

void Missile::Draw(const Camera& camera)
{
	GameObject::Draw(camera);
}

void Missile::DrawUI()
{
}

void Missile::OnCollision(GameObject* gameObject)
{
	//破壊フラグを立てる
	SetIsDestroy(true);
}

void Missile::OnCollisionEnter(GameObject* gameObject)
{
}

void Missile::OnCollisionExit(GameObject* gameObject)
{
}

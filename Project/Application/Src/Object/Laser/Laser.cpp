#include "Laser.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Utilities/GameTimer.h"

void Laser::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();
}

void Laser::Update()
{
	//Transformを取得
	TransformComponent* transformComponent = GetComponent<TransformComponent>();

	//死亡状態ではないとき
	if (!isDead_)
	{
		//イージングのタイマーを進める
		if (easingParameter_ <= 1.0f)
		{
			easingParameter_ += easingSpeed_ * GameTimer::GetDeltaTime();
		}

		//スケールを徐々に大きくする
		transformComponent->worldTransform_.scale_ = Mathf::Lerp({ 0.0f,0.0f,targetScale_.z }, targetScale_, easingParameter_);

		//死亡タイマーを進める
		deathTimer_ += GameTimer::GetDeltaTime();

		//死亡タイマーが生存時間を超えたら死亡させる
		if (deathTimer_ > lifeTime_)
		{
			isDead_ = true;
			easingParameter_ = 0.0f;
		}
	}
	else
	{
		//イージングのタイマーを進める
		easingParameter_ += easingSpeed_ * GameTimer::GetDeltaTime();

		//スケールを徐々に小さくする
		transformComponent->worldTransform_.scale_ = Mathf::Lerp(targetScale_, { 0.0f,0.0f,targetScale_.z }, easingParameter_);

		//イージングが終了したら削除する
		if (easingParameter_ >= 1.0f)
		{
			SetIsDestroy(true);
		}
	}

	//Colliderの更新
	OBBCollider* collider = GetComponent<OBBCollider>();
	collider->SetOrientations(
		{ transformComponent->worldTransform_.matWorld_.m[0][0], transformComponent->worldTransform_.matWorld_.m[0][1], transformComponent->worldTransform_.matWorld_.m[0][2] },
		{ transformComponent->worldTransform_.matWorld_.m[1][0], transformComponent->worldTransform_.matWorld_.m[1][1], transformComponent->worldTransform_.matWorld_.m[1][2] },
		{ transformComponent->worldTransform_.matWorld_.m[2][0], transformComponent->worldTransform_.matWorld_.m[2][1], transformComponent->worldTransform_.matWorld_.m[2][2] }
	);
	collider->SetSize(transformComponent->worldTransform_.scale_);

	//基底クラスの更新
	GameObject::Update();
}

void Laser::Draw(const Camera& camera)
{
	//基底クラスの描画
	GameObject::Draw(camera);
}

void Laser::DrawUI()
{
}

void Laser::OnCollision(GameObject* gameObject)
{
}

void Laser::OnCollisionEnter(GameObject* gameObject)
{
}

void Laser::OnCollisionExit(GameObject* gameObject)
{
}

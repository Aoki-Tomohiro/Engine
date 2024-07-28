#include "Enemy.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/SphereCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"

void Enemy::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();

	//Stateの初期化
	ChangeState(new EnemyStateRoot());

	//パーティクルを生成
	particleSystem_ = ParticleManager::Create("Enemy");
}

void Enemy::Update()
{
	//アニメーションの速度の更新
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(timeScale_);

	//動いていない状態の時
	if (!isMove_)
	{
		//プレイヤーを取得
		if (Player* player = GameObjectManager::GetInstance()->GetGameObject<Player>())
		{	
			//プレイヤーの速度が0でなければ
			if (player->GetVelocity() != Vector3{ 0.0f,0.0f,0.0f })
			{
				//移動フラグを立てる
				isMove_ = true;

				//アニメーションを歩きにする
				ModelComponent* modelComponent = GetComponent<ModelComponent>();
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0");
			}
		}
	}

	//タイトルシーンにいる場合は移動処理をスキップ
	if (!isInTitleScene_)
	{
		//Stateの更新
		state_->Update();
	}

	//回転処理
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transformComponent->worldTransform_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));

	//Colliderを設定
	AABBCollider* collider = GetComponent<AABBCollider>();
	collider->SetDebugDrawEnabled(false);

	//GameObjectの更新
	GameObject::Update();

	//ImGui
	ImGui::Begin("Enemy");
	ImGui::Checkbox("IsDebug", &isDebug_);
	ImGui::DragFloat("AnimationTime", &animationTime_, 0.01f);
	ImGui::End();
}

void Enemy::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
}

void Enemy::DrawUI()
{

}

void Enemy::OnCollision(GameObject* gameObject)
{

}

void Enemy::OnCollisionEnter(GameObject* gameObject)
{

}

void Enemy::OnCollisionExit(GameObject* gameObject)
{

}

const Vector3 Enemy::GetHipWorldPosition()
{
	//腰のJointの座標を取得
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	WorldTransform hipWorldTransform = modelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:Hips");
	Vector3 hipWorldPosition = {
		hipWorldTransform.matWorld_.m[3][0],
		hipWorldTransform.matWorld_.m[3][1],
		hipWorldTransform.matWorld_.m[3][2],
	};
	return hipWorldPosition;
}

const Vector3 Enemy::GetHipLocalPosition()
{
	//ローカルの腰の座標を取得
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	Vector3 hipLocalPosition = GetHipWorldPosition() - transformComponent->GetWorldPosition();
	return hipLocalPosition;
}

void Enemy::ChangeState(IEnemyState* state)
{
	state->SetEnemy(this);
	state->Initialize();
	state_.reset(state);
}

void Enemy::CreateWarningParticle()
{
	//パーティクルを出す
	for (uint32_t i = 0; i < 360; ++i)
	{
		//速度を決める
		TransformComponent* transformComponent = GetComponent<TransformComponent>();
		const float kParticleVelocity = 0.1f;
		const float distance = 1.2f;
		Quaternion rotate = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,0.0f,1.0f }, i * (3.14f / 180.0f));
		Vector3 direction = Mathf::RotateVector({ 0.0f,1.0f,0.0f }, destinationQuaternion_ * rotate);

		//エミッターの生成
		ParticleEmitter* emitter = EmitterBuilder()
			.SetEmitterName("ChargeUp")
			.SetColor({ 0.6f,0.5f,0.0f,1.0f }, { 1.0f,0.5f,0.0f,1.0f })
			.SetEmitterLifeTime(0.1f)
			.SetCount(1)
			.SetFrequency(0.2f)
			.SetLifeTime(0.4f, 0.4f)
			.SetRadius(0.1f)
			.SetScale({ 0.1f,0.1f,0.1f }, { 0.3f,0.3f,0.3f })
			.SetTranslation(GetHipWorldPosition() + direction * distance)
			.SetVelocity(direction * kParticleVelocity, direction * kParticleVelocity)
			.Build();
		particleSystem_->AddParticleEmitter(emitter);
	}
}

Warning* Enemy::CreateBoxWarningObject(const Warning::BoxWarningObjectSettings& warningObjectSettings)
{
	//警告用のオブジェクトを生成
	Warning* warning = GameObjectManager::GetInstance()->CreateGameObject<Warning>();
	warning->SetBoxWarningObjectSettings(warningObjectSettings);

	//Transformの追加
	TransformComponent* warningTransformComponent = warning->AddComponent<TransformComponent>();
	warningTransformComponent->Initialize();
	warningTransformComponent->worldTransform_.rotationType_ = RotationType::Quaternion;

	//Modelの追加
	ModelComponent* warningModelComponent = warning->AddComponent<ModelComponent>();
	switch (warningObjectSettings.warningPrimitive)
	{
	case Warning::kBox:
		warningModelComponent->Initialize("Cube", Transparent);
		break;
	case Warning::kSphere:
		warningModelComponent->Initialize("Sphere", Transparent);
		break;
	}
	warningModelComponent->SetTransformComponent(warningTransformComponent);
	warningModelComponent->GetModel()->GetMaterial(1)->SetColor({ 1.0f,0.0f,0.0f,0.6f });

	//Colliderの追加
	Collider* collider = nullptr;
	switch (warningObjectSettings.warningPrimitive)
	{
	case Warning::kBox:
		collider = warning->AddComponent<OBBCollider>();
		break;
	case Warning::kSphere:
		collider = warning->AddComponent<SphereCollider>();
		break;
	}
	collider->Initialize();
	collider->SetTransformComponent(warningTransformComponent);
	collider->SetCollisionAttribute(CollisionAttributeManager::GetInstance()->GetAttribute("Warning"));
	collider->SetCollisionMask(CollisionAttributeManager::GetInstance()->GetMask("Warning"));

	return warning;
}
#include "Enemy.h"
#include <numbers>

void Enemy::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();

	//TransformComponentの初期化
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.rotationType_ = RotationType::Quaternion;
	destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>) * transformComponent->worldTransform_.quaternion_;

	//ModelComponentの初期化
	ModelComponent* enemyModelComponent = GetComponent<ModelComponent>();
	enemyModelComponent->SetAnimationName("Armature|mixamo.com|Layer0");
}

void Enemy::Update()
{
	//ModelComponentを取得
	modelComponent_ = GetComponent<ModelComponent>();
	modelComponent_->GetModel()->GetAnimation()->SetAnimationSpeed(timeScale_);

	//回転処理
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transformComponent->worldTransform_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));

	//基底クラスの更新
	GameObject::Update();
}

void Enemy::Draw(const Camera& camera)
{
	//基底クラスの描画
	GameObject::Draw(camera);
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

const Vector3 Enemy::GetHipWorldPosition() const
{
	WorldTransform hipWorldTransform = modelComponent_->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:Hips");
	Vector3 hipWorldPosition = {
		hipWorldTransform.matWorld_.m[3][0],
		hipWorldTransform.matWorld_.m[3][1],
		hipWorldTransform.matWorld_.m[3][2],
	};
	return hipWorldPosition;
}
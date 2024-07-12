#include "Weapon.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void Weapon::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();

	//パーティクルシステムの作成
	particleSystem_ = ParticleManager::Create("Weapon");
}

void Weapon::Update()
{
	//フラグのリセット
	isHit_ = false;
	isParrySuccessful_ = false;

	//Colliderの更新
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	Vector3 center = Mathf::TransformNormal(collisionOffset_, transformComponent->worldTransform_.matWorld_);
	OBBCollider* collider = GetComponent<OBBCollider>();
	collider->SetCenter(center);
	collider->SetOrientations(
		{ transformComponent->worldTransform_.matWorld_.m[0][0], transformComponent->worldTransform_.matWorld_.m[0][1], transformComponent->worldTransform_.matWorld_.m[0][2] },
		{ transformComponent->worldTransform_.matWorld_.m[1][0], transformComponent->worldTransform_.matWorld_.m[1][1], transformComponent->worldTransform_.matWorld_.m[1][2] },
		{ transformComponent->worldTransform_.matWorld_.m[2][0], transformComponent->worldTransform_.matWorld_.m[2][1], transformComponent->worldTransform_.matWorld_.m[2][2] }
	);
	collider->SetSize(size_);

	//Gameobjectの更新
	GameObject::Update();

	//ImGui
	ImGui::Begin("Weapon");
	ImGui::DragFloat3("Translate", &transformComponent->worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &transformComponent->worldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("Scale", &transformComponent->worldTransform_.scale_.x, 0.01f);
	ImGui::DragFloat3("Offset", &collisionOffset_.x, 0.01f);
	ImGui::DragFloat3("Size", &size_.x, 0.01f);
	ImGui::Checkbox("IsAttack", &isAttack_);
	ImGui::Checkbox("IsParryable", &isParryable_);
	ImGui::End();
}

void Weapon::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
}

void Weapon::DrawUI()
{

}

void Weapon::OnCollision(GameObject* gameObject)
{

}

void Weapon::OnCollisionEnter(GameObject* gameObject)
{
	//ヒットフラグをtrueにする
	isHit_ = true;

	//パリィ成功
	if (Enemy* enemy = dynamic_cast<Enemy*>(gameObject))
	{
		if (enemy->GetIsAttack())
		{
			if (isParryable_)
			{
				isParrySuccessful_ = true;
			}
		}
	}

	//パーティクルを出す
	OBBCollider* collider = GetComponent<OBBCollider>();
	ParticleEmitter* newEmitter = new ParticleEmitter();
	newEmitter->Initialize("Hit", 1.0f);
	newEmitter->SetTranslate(collider->GetWorldCenter());
	newEmitter->SetCount(400);
	newEmitter->SetColorMin({ 1.0f, 0.2f, 0.2f, 1.0f });
	newEmitter->SetColorMax({ 1.0f, 0.2f, 0.2f, 1.0f });
	newEmitter->SetFrequency(2.0f);
	newEmitter->SetLifeTimeMin(0.2f);
	newEmitter->SetLifeTimeMax(0.4f);
	newEmitter->SetRadius(0.0f);
	newEmitter->SetScaleMin({ 0.2f,0.2f,0.2f });
	newEmitter->SetScaleMax({ 0.3f,0.3f,0.3f });
	newEmitter->SetVelocityMin({ -0.6f,-0.6f,-0.6f });
	newEmitter->SetVelocityMax({ 0.6f,0.6f,0.6f });
	particleSystem_->AddParticleEmitter(newEmitter);
}

void Weapon::OnCollisionExit(GameObject* gameObject)
{

}

void Weapon::SetParent(const WorldTransform* worldTransform)
{
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.parent_ = worldTransform;
}
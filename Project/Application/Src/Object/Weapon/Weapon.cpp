#include "Weapon.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Collision/OBBCollider.h"

void Weapon::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();

	//パーティクルシステムの作成
	particleSystem_ = ParticleManager::Create("Weapon");
}

void Weapon::Update()
{
	//ヒットフラグをリセット
	isHit_ = false;

	//Colliderの更新
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	Vector3 offset = Mathf::TransformNormal(collisionOffset_, transformComponent->worldTransform_.matWorld_);
	Vector3 center = transformComponent->GetWorldPosition() + offset;
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
	ImGui::End();
}

void Weapon::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
}

void Weapon::OnCollision(GameObject* gameObject)
{

}

void Weapon::OnCollisionEnter(GameObject* gameObject)
{
	//ヒットフラグをtrueにする
	isHit_ = true;

	//パーティクルを出す
	OBBCollider* collider = GetComponent<OBBCollider>();
	ParticleEmitter* newEmitter = new ParticleEmitter();
	newEmitter->Initialize("Hit", 1.0f);
	newEmitter->SetTranslate(collider->GetCenter());
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

void Weapon::SetParent(const TransformComponent* parentTransformComponent)
{
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.parent_ = &parentTransformComponent->worldTransform_;
}
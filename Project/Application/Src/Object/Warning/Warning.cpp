#include "Warning.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/SphereCollider.h"

void Warning::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();
}

void Warning::Update()
{
	//Transformの更新
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	Vector3 offset = Mathf::RotateVector(warningObjectSettings_.offset, warningObjectSettings_.quaternion);
	transformComponent->worldTransform_.translation_ = warningObjectSettings_.position + offset;
	transformComponent->worldTransform_.quaternion_ = warningObjectSettings_.quaternion;
	transformComponent->worldTransform_.scale_ = warningObjectSettings_.scale;

	//Colliderの更新
	OBBCollider* boxCollider = nullptr;
	SphereCollider* sphereCollider = nullptr;
	switch (warningObjectSettings_.warningPrimitive)
	{
	case kBox:
		boxCollider = GetComponent<OBBCollider>();
		boxCollider->SetCenter(warningObjectSettings_.colliderCenter);
		boxCollider->SetSize(warningObjectSettings_.colliderSize);
		boxCollider->SetOrientations(
			{ transformComponent->worldTransform_.matWorld_.m[0][0], transformComponent->worldTransform_.matWorld_.m[0][1], transformComponent->worldTransform_.matWorld_.m[0][2] },
			{ transformComponent->worldTransform_.matWorld_.m[1][0], transformComponent->worldTransform_.matWorld_.m[1][1], transformComponent->worldTransform_.matWorld_.m[1][2] },
			{ transformComponent->worldTransform_.matWorld_.m[2][0], transformComponent->worldTransform_.matWorld_.m[2][1], transformComponent->worldTransform_.matWorld_.m[2][2] }
		);
		break;
	case kSphere:
		sphereCollider = GetComponent<SphereCollider>();
		sphereCollider->SetCenter(warningObjectSettings_.colliderCenter);
		sphereCollider->SetRadius(warningObjectSettings_.radius);
		break;
	}

	//Gameobjectの更新
	GameObject::Update();

	//ImGui
	ImGui::Begin("Warning");
	ImGui::DragFloat3("Translation", &warningObjectSettings_.position.x, 0.01f);
	ImGui::DragFloat4("Quaternion", &warningObjectSettings_.quaternion.x, 0.01f);
	ImGui::DragFloat3("Scale", &warningObjectSettings_.scale.x, 0.01f);
	ImGui::DragFloat3("Offset", &warningObjectSettings_.offset.x, 0.01f);
	ImGui::DragFloat3("ColliderCenter", &warningObjectSettings_.colliderCenter.x, 0.01f);
	ImGui::DragFloat3("ColliderSize", &warningObjectSettings_.colliderSize.x, 0.01f);
	ImGui::DragFloat("ColliderRadius", &warningObjectSettings_.radius, 0.01f);
	ImGui::End();
}

void Warning::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
}

void Warning::DrawUI()
{

}

void Warning::OnCollision(GameObject* gameObject)
{
}

void Warning::OnCollisionEnter(GameObject* gameObject)
{
}

void Warning::OnCollisionExit(GameObject* gameObject)
{
}
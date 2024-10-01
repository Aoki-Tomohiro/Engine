#include "BackGroundObject.h"
#include "Engine/Components/Model/ModelComponent.h"

void BackGroundObject::Initialize()
{
	//基底クラスの初期化
	//GameObject::Initialize();
}

void BackGroundObject::Update()
{
	//基底クラスの更新
	//GameObject::Update();
}

void BackGroundObject::Draw(const Camera& camera)
{
	//基底クラスの描画
	//GameObject::Draw(camera);
}

void BackGroundObject::OnCollision(GameObject* gameObject)
{
}

void BackGroundObject::OnCollisionEnter(GameObject* gameObject)
{
}

void BackGroundObject::OnCollisionExit(GameObject* gameObject)
{
}
#include "BreakableObject.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Collision/AABBCollider.h"

void BreakableObject::Initialize()
{
	//名前を設定
	SetName("BreakableObject");

	//基底クラスの呼び出し
	GameObject::Initialize();
}

void BreakableObject::OnCollision(GameObject*)
{

}
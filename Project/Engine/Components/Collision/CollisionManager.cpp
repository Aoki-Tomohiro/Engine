/**
 * @file CollisionManager.cpp
 * @brief 衝突判定を行うファイル
 * @author 青木智滉
 * @date
 */

#include "CollisionManager.h"
#include "CollisionConfig.h"
#include "Engine/Math/MathFunction.h"
#include <algorithm>

void CollisionManager::ClearColliderList()
{
	//コライダーリストをクリア
	colliders_.clear();
}

void CollisionManager::SetColliderList(Collider* collider)
{
	//コライダーリストに登録
	colliders_.push_back(collider);
}

void CollisionManager::CheckAllCollisions()
{
	//リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA)
	{
		//イテレータAからコライダーAを取得する
		Collider* colliderA = *itrA;
		//イテレータBはイテレータAの次の要素から回す(重複判定を回避)
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB)
		{
			//イテレータBからコライダーBを取得する
			Collider* colliderB = *itrB;
			//ベアの当たり判定
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void CollisionManager::CheckCollisionPair(Collider* collider1, Collider* collider2)
{
	//nullチェック
	if (!collider1 || !collider2)
	{
		return;
	}

	//衝突フィルタリング
	if ((collider1->GetCollisionAttribute() & collider2->GetCollisionMask()) == 0 ||
		(collider2->GetCollisionAttribute() & collider1->GetCollisionMask()) == 0)
	{
		return;
	}

	//衝突判定が無効化された場合
	if (!collider1->GetCollisionEnabled() || !collider2->GetCollisionEnabled())
	{
		if (collider1->IsCollidingWith(collider2))
		{
			collider1->RemoveCollision(collider2);
			collider2->RemoveCollision(collider1);
			collider1->OnCollisionExit(collider2->owner_);
			collider2->OnCollisionExit(collider1->owner_);
		}
		return;
	}

	bool isColliding = false;

	//衝突判定
	if (dynamic_cast<SphereCollider*>(collider1) && dynamic_cast<SphereCollider*>(collider2))
	{
		isColliding = CheckSphereSphereCollision(static_cast<SphereCollider*>(collider1), static_cast<SphereCollider*>(collider2));
	}
	else if (dynamic_cast<AABBCollider*>(collider1) && dynamic_cast<AABBCollider*>(collider2))
	{
		isColliding = CheckAABBAABBCollision(static_cast<AABBCollider*>(collider1), static_cast<AABBCollider*>(collider2));
	}
	else if (dynamic_cast<OBBCollider*>(collider1) && dynamic_cast<OBBCollider*>(collider2))
	{
		isColliding = CheckOBBOBBCollision(static_cast<OBBCollider*>(collider1), static_cast<OBBCollider*>(collider2));
	}
	else if (dynamic_cast<SphereCollider*>(collider1) && dynamic_cast<AABBCollider*>(collider2))
	{
		isColliding = CheckSphereAABBCollision(static_cast<SphereCollider*>(collider1), static_cast<AABBCollider*>(collider2));
	}
	else if (dynamic_cast<AABBCollider*>(collider1) && dynamic_cast<SphereCollider*>(collider2))
	{
		isColliding = CheckSphereAABBCollision(static_cast<SphereCollider*>(collider2), static_cast<AABBCollider*>(collider1));
	}
	else if (dynamic_cast<SphereCollider*>(collider1) && dynamic_cast<OBBCollider*>(collider2))
	{
		isColliding = CheckSphereOBBCollision(static_cast<SphereCollider*>(collider1), static_cast<OBBCollider*>(collider2));
	}
	else if (dynamic_cast<OBBCollider*>(collider1) && dynamic_cast<SphereCollider*>(collider2))
	{
		isColliding = CheckSphereOBBCollision(static_cast<SphereCollider*>(collider2), static_cast<OBBCollider*>(collider1));
	}
	else if (dynamic_cast<AABBCollider*>(collider1) && dynamic_cast<OBBCollider*>(collider2))
	{
		isColliding = CheckAABBOBBCollision(static_cast<AABBCollider*>(collider1), static_cast<OBBCollider*>(collider2));
	}
	else if (dynamic_cast<OBBCollider*>(collider1) && dynamic_cast<AABBCollider*>(collider2))
	{
		isColliding = CheckAABBOBBCollision(static_cast<AABBCollider*>(collider2), static_cast<OBBCollider*>(collider1));
	}

	if (isColliding)
	{
		if (!collider1->IsCollidingWith(collider2))
		{
			collider1->AddCollision(collider2);
			collider2->AddCollision(collider1);
			collider1->OnCollisionEnter(collider2->owner_);
			collider2->OnCollisionEnter(collider1->owner_);
		}
		collider1->OnCollision(collider2->owner_);
		collider2->OnCollision(collider1->owner_);
	}
	else
	{
		if (collider1->IsCollidingWith(collider2))
		{
			collider1->RemoveCollision(collider2);
			collider2->RemoveCollision(collider1);
			collider1->OnCollisionExit(collider2->owner_);
			collider2->OnCollisionExit(collider1->owner_);
		}
	}
}

bool CollisionManager::CheckSphereSphereCollision(const SphereCollider* sphere1, const SphereCollider* sphere2)
{
	float distance = Mathf::Length(sphere1->GetWorldCenter() - sphere2->GetWorldCenter());

	float radiusSum = sphere1->GetRadius() + sphere2->GetRadius();

	return distance <= radiusSum;
}

bool CollisionManager::CheckAABBAABBCollision(const AABBCollider* aabb1, const AABBCollider* aabb2)
{
	Vector3 min1 = aabb1->GetWorldCenter() + aabb1->GetMin();
	Vector3 max1 = aabb1->GetWorldCenter() + aabb1->GetMax();
	Vector3 min2 = aabb2->GetWorldCenter() + aabb2->GetMin();
	Vector3 max2 = aabb2->GetWorldCenter() + aabb2->GetMax();

	if (max1.x < min2.x || min1.x > max2.x) return false;

	if (max1.y < min2.y || min1.y > max2.y) return false;

	if (max1.z < min2.z || min1.z > max2.z) return false;

	return true;
}

bool CollisionManager::CheckOBBOBBCollision(const OBBCollider* obb1, const OBBCollider* obb2)
{
	Vector3 NAe1 = obb1->GetOrientation(0);
	Vector3 Ae1 = NAe1 * obb1->GetSize().x;
	Vector3 NAe2 = obb1->GetOrientation(1);
	Vector3 Ae2 = NAe2 * obb1->GetSize().y;
	Vector3 NAe3 = obb1->GetOrientation(2);
	Vector3 Ae3 = NAe3 * obb1->GetSize().z;

	Vector3 NBe1 = obb2->GetOrientation(0);
	Vector3 Be1 = NBe1 * obb2->GetSize().x;
	Vector3 NBe2 = obb2->GetOrientation(1);
	Vector3 Be2 = NBe2 * obb2->GetSize().y;
	Vector3 NBe3 = obb2->GetOrientation(2);
	Vector3 Be3 = NBe3 * obb2->GetSize().z;

	Vector3 Interval = obb1->GetWorldCenter() - obb2->GetWorldCenter();

	auto LenSegOnSeparateAxis = [](const Vector3* Sep, const Vector3* e1, const Vector3* e2, const Vector3* e3) -> float {
		float r1 = fabs(Mathf::Dot(*Sep, *e1));
		float r2 = fabs(Mathf::Dot(*Sep, *e2));
		float r3 = e3 ? (fabs(Mathf::Dot(*Sep, *e3))) : 0;
		return r1 + r2 + r3;
		};

	//文理軸 Ae1
	float rA = Mathf::Length(Ae1);
	float rB = LenSegOnSeparateAxis(&NAe1, &Be1, &Be2, &Be3);
	float L = fabs(Mathf::Dot(Interval, NAe1));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Ae2
	rA = Mathf::Length(Ae2);
	rB = LenSegOnSeparateAxis(&NAe2, &Be1, &Be2, &Be3);
	L = fabs(Mathf::Dot(Interval, NAe2));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Ae3
	rA = Mathf::Length(Ae3);
	rB = LenSegOnSeparateAxis(&NAe3, &Be1, &Be2, &Be3);
	L = fabs(Mathf::Dot(Interval, NAe3));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Be1
	rA = LenSegOnSeparateAxis(&NBe1, &Ae1, &Ae2, &Ae3);
	rB = Mathf::Length(Be1);
	L = fabs(Mathf::Dot(Interval, NBe1));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Be2
	rA = LenSegOnSeparateAxis(&NBe2, &Ae1, &Ae2, &Ae3);
	rB = Mathf::Length(Be2);
	L = fabs(Mathf::Dot(Interval, NBe2));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Be3
	rA = LenSegOnSeparateAxis(&NBe3, &Ae1, &Ae2, &Ae3);
	rB = Mathf::Length(Be3);
	L = fabs(Mathf::Dot(Interval, NBe3));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C11
	Vector3 Cross = Mathf::Cross(NAe1, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C12
	Cross = Mathf::Cross(NAe1, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C13
	Cross = Mathf::Cross(NAe1, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C21
	Cross = Mathf::Cross(NAe2, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C22
	Cross = Mathf::Cross(NAe2, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C23
	Cross = Mathf::Cross(NAe2, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C31
	Cross = Mathf::Cross(NAe3, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C32
	Cross = Mathf::Cross(NAe3, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C33
	Cross = Mathf::Cross(NAe3, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	return true;
}

bool CollisionManager::CheckSphereAABBCollision(const SphereCollider* sphere1, const AABBCollider* aabb1)
{
	//最近接点を求める
	Vector3 closestPoint{
		std::clamp(sphere1->GetWorldCenter().x,aabb1->GetWorldCenter().x + aabb1->GetMin().x,aabb1->GetWorldCenter().x + aabb1->GetMax().x),
		std::clamp(sphere1->GetWorldCenter().y,aabb1->GetWorldCenter().y + aabb1->GetMin().y,aabb1->GetWorldCenter().y + aabb1->GetMax().y),
		std::clamp(sphere1->GetWorldCenter().z,aabb1->GetWorldCenter().z + aabb1->GetMin().z,aabb1->GetWorldCenter().z + aabb1->GetMax().z)
	};

	//最近接点と球の中心との距離を求める
	float distance = Mathf::Length(closestPoint - sphere1->GetWorldCenter());

	//距離が半径よりも小さければ衝突
	if (distance <= sphere1->GetRadius())
	{
		return true;
	}

	return false;
}

bool CollisionManager::CheckSphereOBBCollision(const SphereCollider* sphere1, const OBBCollider* obb1)
{
	Vector3 sphereCenter = sphere1->GetWorldCenter();
	Vector3 obbCenter = obb1->GetWorldCenter();
	Vector3 obbSize = obb1->GetSize();

	//球体の中心からOBB中心へのベクトル
	Vector3 d = sphereCenter - obbCenter;

	//OBBの各軸方向ベクトルと最大サイズ
	Vector3 axes[3];
	float axisSizes[3] = { obbSize.x, obbSize.y, obbSize.z };
	for (int i = 0; i < 3; ++i)
	{
		axes[i] = obb1->GetOrientation(i);
	}

	//最近接点を計算
	Vector3 closestPoint = obbCenter;
	for (int i = 0; i < 3; ++i)
	{
		float distance = Mathf::Dot(d, axes[i]);
		float clampedDistance = std::max<float>(-axisSizes[i], std::min<float>(distance, axisSizes[i]));
		closestPoint = closestPoint + (axes[i] * clampedDistance);
	}

	//最近接点と球体の中心との距離を計算
	Vector3 v = closestPoint - sphereCenter;
	float distanceSquared = Mathf::Dot(v, v);
	float radiusSquared = sphere1->GetRadius() * sphere1->GetRadius();

	return distanceSquared <= radiusSquared;
}

bool CollisionManager::CheckAABBOBBCollision(const AABBCollider* aabb1, const OBBCollider* obb1)
{
	Vector3 aabbCenter = (aabb1->GetWorldCenter() + aabb1->GetMin() + aabb1->GetWorldCenter() + aabb1->GetMax()) * 0.5f;

	float aabbHalfSize[3] = {
	0.5f * (aabb1->GetMax().x - aabb1->GetMin().x),
	0.5f * (aabb1->GetMax().y - aabb1->GetMin().y),
	0.5f * (aabb1->GetMax().z - aabb1->GetMin().z),
	};

	Vector3 NAe1 = { 1.0f,0.0f,0.0f };
	Vector3 Ae1 = NAe1 * aabbHalfSize[0];
	Vector3 NAe2 = { 0.0f,1.0f,0.0f };
	Vector3 Ae2 = NAe2 * aabbHalfSize[1];
	Vector3 NAe3 = { 0.0f,0.0f,1.0f };
	Vector3 Ae3 = NAe3 * aabbHalfSize[2];

	Vector3 NBe1 = obb1->GetOrientation(0);
	Vector3 Be1 = NBe1 * obb1->GetSize().x;
	Vector3 NBe2 = obb1->GetOrientation(1);
	Vector3 Be2 = NBe2 * obb1->GetSize().y;
	Vector3 NBe3 = obb1->GetOrientation(2);
	Vector3 Be3 = NBe3 * obb1->GetSize().z;

	Vector3 Interval = aabbCenter - obb1->GetWorldCenter();

	auto LenSegOnSeparateAxis = [](const Vector3* Sep, const Vector3* e1, const Vector3* e2, const Vector3* e3) -> float {
		float r1 = fabs(Mathf::Dot(*Sep, *e1));
		float r2 = fabs(Mathf::Dot(*Sep, *e2));
		float r3 = e3 ? (fabs(Mathf::Dot(*Sep, *e3))) : 0;
		return r1 + r2 + r3;
	};

	//文理軸 Ae1
	float rA = Mathf::Length(Ae1);
	float rB = LenSegOnSeparateAxis(&NAe1, &Be1, &Be2, &Be3);
	float L = fabs(Mathf::Dot(Interval, NAe1));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Ae2
	rA = Mathf::Length(Ae2);
	rB = LenSegOnSeparateAxis(&NAe2, &Be1, &Be2, &Be3);
	L = fabs(Mathf::Dot(Interval, NAe2));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Ae3
	rA = Mathf::Length(Ae3);
	rB = LenSegOnSeparateAxis(&NAe3, &Be1, &Be2, &Be3);
	L = fabs(Mathf::Dot(Interval, NAe3));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Be1
	rA = LenSegOnSeparateAxis(&NBe1, &Ae1, &Ae2, &Ae3);
	rB = Mathf::Length(Be1);
	L = fabs(Mathf::Dot(Interval, NBe1));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Be2
	rA = LenSegOnSeparateAxis(&NBe2, &Ae1, &Ae2, &Ae3);
	rB = Mathf::Length(Be2);
	L = fabs(Mathf::Dot(Interval, NBe2));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : Be3
	rA = LenSegOnSeparateAxis(&NBe3, &Ae1, &Ae2, &Ae3);
	rB = Mathf::Length(Be3);
	L = fabs(Mathf::Dot(Interval, NBe3));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C11
	Vector3 Cross = Mathf::Cross(NAe1, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C12
	Cross = Mathf::Cross(NAe1, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C13
	Cross = Mathf::Cross(NAe1, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C21
	Cross = Mathf::Cross(NAe2, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C22
	Cross = Mathf::Cross(NAe2, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C23
	Cross = Mathf::Cross(NAe2, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C31
	Cross = Mathf::Cross(NAe3, NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C32
	Cross = Mathf::Cross(NAe3, NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	// 分離軸 : C33
	Cross = Mathf::Cross(NAe3, NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
	L = fabs(Mathf::Dot(Interval, Cross));
	if (L > rA + rB)
	{
		return false;
	}

	return true;
}
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

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) 
{
	//衝突フィルタリング
	if ((colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) == 0 ||
		(colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask()) == 0)
	{
		return;
	}

	//球と球の判定
	if (((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0) ||
		((colliderB->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0 && (colliderA->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0))
	{
		//コライダーAのワールド座標を取得
		Vector3 posA = colliderA->GetWorldPosition();
		//コライダーBのワールド座標を取得
		Vector3 posB = colliderB->GetWorldPosition();
		//コライダーAとコライダーBの距離を計算
		float distance = Mathf::Length(posA - posB);
		//球と球の交差判定
		if (distance <= colliderA->GetRadius() + colliderB->GetRadius()) 
		{
			//コライダーAの衝突時コールバックを呼び出す
			colliderA->OnCollision(colliderB);
			//コライダーBの衝突時コールバックを呼び出す
			colliderB->OnCollision(colliderA);
		}
	}

	//AABBとAABBの判定
	if (((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0) ||
		((colliderB->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0 && (colliderA->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0)) 
	{
		//コライダーAのワールド座標を取得
		Vector3 posA = colliderA->GetWorldPosition();
		//コライダーBのワールド座標を取得
		Vector3 posB = colliderB->GetWorldPosition();
		//コライダーAのAABBを取得
		AABB aabbA = colliderA->GetAABB();
		//コライダーBのAABBを取得
		AABB aabbB = colliderB->GetAABB();
		if (posA.x + aabbA.min.x <= posB.x + aabbB.max.x && posA.x + aabbA.max.x >= posB.x + aabbB.min.x &&
			posA.y + aabbA.min.y <= posB.y + aabbB.max.y && posA.y + aabbA.max.y >= posB.y + aabbB.min.y &&
			posA.z + aabbA.min.z <= posB.z + aabbB.max.z && posA.z + aabbA.max.z >= posB.z + aabbB.min.z) 
		{
			//コライダーAの衝突時コールバックを呼び出す
			colliderA->OnCollision(colliderB);
			//コライダーBの衝突時コールバックを呼び出す
			colliderB->OnCollision(colliderA);
		}
	}

	//球とAABBの判定
	if (((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0) ||
		((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveSphere) != 0))
	{
		//コライダーAのワールド座標を取得
		Vector3 posA = colliderA->GetWorldPosition();
		//コライダーBのワールド座標を取得
		Vector3 posB = colliderB->GetWorldPosition();
		//コライダーAのAABBを取得
		AABB aabbA = colliderA->GetAABB();
		//コライダーBのAABBを取得
		AABB aabbB = colliderB->GetAABB();

		//コライダーAがSphereの場合
		if (colliderA->GetCollisionPrimitive() & kCollisionPrimitiveSphere) 
		{
			//最近接点を求める
			Vector3 closestPoint{
				std::clamp(posA.x,posB.x + aabbB.min.x,posB.x + aabbB.max.x),
				std::clamp(posA.y,posB.y + aabbB.min.y,posB.y + aabbB.max.y),
				std::clamp(posA.z,posB.z + aabbB.min.z,posB.z + aabbB.max.z) };
			//最近接点と球の中心との距離を求める
			float distance = Mathf::Length(closestPoint - posA);
			//距離が半径よりも小さければ衝突
			if (distance <= colliderA->GetRadius())
			{
				//コライダーAの衝突時コールバックを呼び出す
				colliderA->OnCollision(colliderB);
				//コライダーBの衝突時コールバックを呼び出す
				colliderB->OnCollision(colliderA);
			}
		}
		else if (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveSphere)
		{
			//最近接点を求める
			Vector3 closestPoint{
				std::clamp(posB.x,posA.x + aabbA.min.x,posA.x + aabbA.max.x),
				std::clamp(posB.y,posA.y + aabbA.min.y,posA.y + aabbA.max.y),
				std::clamp(posB.z,posA.z + aabbA.min.z,posA.z + aabbA.max.z) };
			//最近接点と球の中心との距離を求める
			float distance = Mathf::Length(closestPoint - posB);
			//距離が半径よりも小さければ衝突
			if (distance <= colliderB->GetRadius()) 
			{
				//コライダーAの衝突時コールバックを呼び出す
				colliderA->OnCollision(colliderB);
				//コライダーBの衝突時コールバックを呼び出す
				colliderB->OnCollision(colliderA);
			}
		}
	}

	//OBBとAABBの判定
	if (((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveOBB) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0) ||
		((colliderA->GetCollisionPrimitive() & kCollisionPrimitiveAABB) != 0 && (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveOBB) != 0))
	{
		//ColliderAがAABBの場合
		if (colliderA->GetCollisionPrimitive() & kCollisionPrimitiveAABB)
		{
			AABB aabb = {
				.min{colliderA->GetWorldPosition().x + colliderA->GetAABB().min.x,colliderA->GetWorldPosition().y + colliderA->GetAABB().min.y,colliderA->GetWorldPosition().z + colliderA->GetAABB().min.z},
				.max{colliderA->GetWorldPosition().x + colliderA->GetAABB().max.x,colliderA->GetWorldPosition().y + colliderA->GetAABB().max.y,colliderA->GetWorldPosition().z + colliderA->GetAABB().max.z},
			};

			Vector3 halfsize;
			halfsize.x = (aabb.max.x - aabb.min.x) / 2.0f;
			halfsize.y = (aabb.max.y - aabb.min.y) / 2.0f;
			halfsize.z = (aabb.max.z - aabb.min.z) / 2.0f;

			OBB obb = colliderB->GetOBB();

			Vector3 NAe1 = { 1.0f,0.0f,0.0f };
			Vector3 Ae1 = NAe1 * halfsize.x;
			Vector3 NAe2 = { 0.0f,1.0f,0.0f };
			Vector3 Ae2 = NAe2 * halfsize.y;
			Vector3 NAe3 = { 0.0f,0.0f,1.0f };
			Vector3 Ae3 = NAe3 * halfsize.z;

			Vector3 NBe1 = obb.orientations[0];
			Vector3 Be1 = NBe1 * obb.size.x;
			Vector3 NBe2 = obb.orientations[1];
			Vector3 Be2 = NBe2 * obb.size.y;
			Vector3 NBe3 = obb.orientations[2];
			Vector3 Be3 = NBe3 * obb.size.z;

			Vector3 Interval = colliderA->GetWorldPosition() - obb.center;

			//文理軸 Ae1
			float rA = Mathf::Length(Ae1);
			float rB = LenSegOnSeparateAxis(&NAe1, &Be1, &Be2, &Be3);
			float L = fabs(Mathf::Dot(Interval, NAe1));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : Ae2
			rA = Mathf::Length(Ae2);
			rB = LenSegOnSeparateAxis(&NAe2, &Be1, &Be2, &Be3);
			L = fabs(Mathf::Dot(Interval, NAe2));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : Ae3
			rA = Mathf::Length(Ae3);
			rB = LenSegOnSeparateAxis(&NAe3, &Be1, &Be2, &Be3);
			L = fabs(Mathf::Dot(Interval, NAe3));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : Be1
			rA = LenSegOnSeparateAxis(&NBe1, &Ae1, &Ae2, &Ae3);
			rB = Mathf::Length(Be1);
			L = fabs(Mathf::Dot(Interval, NBe1));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : Be2
			rA = LenSegOnSeparateAxis(&NBe2, &Ae1, &Ae2, &Ae3);
			rB = Mathf::Length(Be2);
			L = fabs(Mathf::Dot(Interval, NBe2));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : Be3
			rA = LenSegOnSeparateAxis(&NBe3, &Ae1, &Ae2, &Ae3);
			rB = Mathf::Length(Be3);
			L = fabs(Mathf::Dot(Interval, NBe3));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C11
			Vector3 Cross = Mathf::Cross(NAe1, NBe1);
			rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C12
			Cross = Mathf::Cross(NAe1, NBe2);
			rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C13
			Cross = Mathf::Cross(NAe1, NBe3);
			rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C21
			Cross = Mathf::Cross(NAe2, NBe1);
			rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C22
			Cross = Mathf::Cross(NAe2, NBe2);
			rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C23
			Cross = Mathf::Cross(NAe2, NBe3);
			rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C31
			Cross = Mathf::Cross(NAe3, NBe1);
			rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C32
			Cross = Mathf::Cross(NAe3, NBe2);
			rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C33
			Cross = Mathf::Cross(NAe3, NBe3);
			rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			//コライダーAの衝突時コールバックを呼び出す
			colliderA->OnCollision(colliderB);
			//コライダーBの衝突時コールバックを呼び出す
			colliderB->OnCollision(colliderA);
		}
		//ColliderBがAABBの場合
		else if (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveAABB)
		{
			AABB aabb = {
	             .min{colliderB->GetWorldPosition().x + colliderB->GetAABB().min.x,colliderB->GetWorldPosition().y + colliderB->GetAABB().min.y,colliderB->GetWorldPosition().z + colliderB->GetAABB().min.z},
				 .max{colliderB->GetWorldPosition().x + colliderB->GetAABB().max.x,colliderB->GetWorldPosition().y + colliderB->GetAABB().max.y,colliderB->GetWorldPosition().z + colliderB->GetAABB().max.z},
			};

			Vector3 halfsize;
			halfsize.x = (aabb.max.x - aabb.min.x) / 2.0f;
			halfsize.y = (aabb.max.y - aabb.min.y) / 2.0f;
			halfsize.z = (aabb.max.z - aabb.min.z) / 2.0f;

			OBB obb = colliderA->GetOBB();

			Vector3 NAe1 = { 1.0f,0.0f,0.0f };
			Vector3 Ae1 = NAe1 * halfsize.x;
			Vector3 NAe2 = { 0.0f,1.0f,0.0f };
			Vector3 Ae2 = NAe2 * halfsize.y;
			Vector3 NAe3 = { 0.0f,0.0f,1.0f };
			Vector3 Ae3 = NAe3 * halfsize.z;

			Vector3 NBe1 = obb.orientations[0];
			Vector3 Be1 = NBe1 * obb.size.x;
			Vector3 NBe2 = obb.orientations[1];
			Vector3 Be2 = NBe2 * obb.size.y;
			Vector3 NBe3 = obb.orientations[2];
			Vector3 Be3 = NBe3 * obb.size.z;

			Vector3 Interval = colliderB->GetWorldPosition() - obb.center;

			//文理軸 Ae1
			float rA = Mathf::Length(Ae1);
			float rB = LenSegOnSeparateAxis(&NAe1, &Be1, &Be2, &Be3);
			float L = fabs(Mathf::Dot(Interval, NAe1));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : Ae2
			rA = Mathf::Length(Ae2);
			rB = LenSegOnSeparateAxis(&NAe2, &Be1, &Be2, &Be3);
			L = fabs(Mathf::Dot(Interval, NAe2));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : Ae3
			rA = Mathf::Length(Ae3);
			rB = LenSegOnSeparateAxis(&NAe3, &Be1, &Be2, &Be3);
			L = fabs(Mathf::Dot(Interval, NAe3));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : Be1
			rA = LenSegOnSeparateAxis(&NBe1, &Ae1, &Ae2, &Ae3);
			rB = Mathf::Length(Be1);
			L = fabs(Mathf::Dot(Interval, NBe1));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : Be2
			rA = LenSegOnSeparateAxis(&NBe2, &Ae1, &Ae2, &Ae3);
			rB = Mathf::Length(Be2);
			L = fabs(Mathf::Dot(Interval, NBe2));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : Be3
			rA = LenSegOnSeparateAxis(&NBe3, &Ae1, &Ae2, &Ae3);
			rB = Mathf::Length(Be3);
			L = fabs(Mathf::Dot(Interval, NBe3));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C11
			Vector3 Cross = Mathf::Cross(NAe1, NBe1);
			rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C12
			Cross = Mathf::Cross(NAe1, NBe2);
			rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C13
			Cross = Mathf::Cross(NAe1, NBe3);
			rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C21
			Cross = Mathf::Cross(NAe2, NBe1);
			rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C22
			Cross = Mathf::Cross(NAe2, NBe2);
			rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C23
			Cross = Mathf::Cross(NAe2, NBe3);
			rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C31
			Cross = Mathf::Cross(NAe3, NBe1);
			rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C32
			Cross = Mathf::Cross(NAe3, NBe2);
			rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			// 分離軸 : C33
			Cross = Mathf::Cross(NAe3, NBe3);
			rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2, 0);
			rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2, 0);
			L = fabs(Mathf::Dot(Interval, Cross));
			if (L > rA + rB)
			{
				return;
			}

			//コライダーAの衝突時コールバックを呼び出す
			colliderA->OnCollision(colliderB);
			//コライダーBの衝突時コールバックを呼び出す
			colliderB->OnCollision(colliderA);
		}
	}
}

float CollisionManager::LenSegOnSeparateAxis(Vector3* Sep, Vector3* e1, Vector3* e2, Vector3* e3)
{
	float r1 = fabs(Mathf::Dot(*Sep, *e1));
	float r2 = fabs(Mathf::Dot(*Sep, *e2));
	float r3 = e3 ? (fabs(Mathf::Dot(*Sep, *e3))) : 0;
	return r1 + r2 + r3;
}
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
		//コライダーAのSphereを作成
		Sphere sphereA = { .center{posA},.radius{colliderA->GetRadius()} };
		//コライダーBのSphereを作成
		Sphere sphereB = { .center{posB},.radius{colliderB->GetRadius()} };
		//衝突判定
		if (CheckCollisionSphere(sphereA, sphereB))
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
		AABB aabbA = { .min{posA + colliderA->GetAABB().min},.max{posA + colliderA->GetAABB().max} };
		//コライダーBのAABBを取得
		AABB aabbB = { .min{posB + colliderB->GetAABB().min},.max{posB + colliderB->GetAABB().max} };
		//衝突判定
		if (CheckCollisionAABB(aabbA,aabbB))
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

		//コライダーAがSphereの場合
		if (colliderA->GetCollisionPrimitive() & kCollisionPrimitiveSphere) 
		{
			//コライダーAのSphereを作成
			Sphere sphere = { .center{posA},.radius{colliderA->GetRadius()} };
			//コライダーBのAABBを取得
			AABB aabb = { .min{posB + colliderB->GetAABB().min},.max{posB + colliderB->GetAABB().max} };
			//衝突判定
			if (CheckCollisionSphereAABB(sphere,aabb))
			{
				//コライダーAの衝突時コールバックを呼び出す
				colliderA->OnCollision(colliderB);
				//コライダーBの衝突時コールバックを呼び出す
				colliderB->OnCollision(colliderA);
			}
		}
		else if (colliderB->GetCollisionPrimitive() & kCollisionPrimitiveSphere)
		{
			//コライダーAのSphereを作成
			Sphere sphere = { .center{posB},.radius{colliderB->GetRadius()} };
			//コライダーBのAABBを取得
			AABB aabb = { .min{posA + colliderA->GetAABB().min},.max{posA + colliderA->GetAABB().max} };
			//衝突判定
			if (CheckCollisionSphereAABB(sphere, aabb))
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
		//コライダーAがAABBの場合
		if (colliderA->GetCollisionAttribute() & kCollisionPrimitiveAABB)
		{
			//コライダーAのAABBを取得
			AABB aabb = { .min{colliderA->GetWorldPosition() + colliderA->GetAABB().min},.max{colliderA->GetWorldPosition() + colliderA->GetAABB().max}, };
			//コライダーBのOBBを取得
			OBB obb = colliderB->GetOBB();

			//衝突判定
			if (CheckCollisionAABBOBB(aabb, obb))
			{
				//コライダーAの衝突時コールバックを呼び出す
				colliderA->OnCollision(colliderB);
				//コライダーBの衝突時コールバックを呼び出す
				colliderB->OnCollision(colliderA);
			}
		}
		//ColliderBがAABBの場合
		else if (colliderB->GetCollisionAttribute() & kCollisionPrimitiveAABB)
		{
			//コライダーBのAABBを取得
			AABB aabb = { .min{colliderB->GetWorldPosition() + colliderB->GetAABB().min},.max{colliderB->GetWorldPosition() + colliderB->GetAABB().max}, };
			//コライダーBのOBBを取得
			OBB obb = colliderA->GetOBB();

			//衝突判定
			if (CheckCollisionAABBOBB(aabb, obb))
			{
				//コライダーAの衝突時コールバックを呼び出す
				colliderA->OnCollision(colliderB);
				//コライダーBの衝突時コールバックを呼び出す
				colliderB->OnCollision(colliderA);
			}
		}
	}
}

bool CollisionManager::CheckCollisionSphere(const Sphere& sphereA, const Sphere& sphereB)
{
	//コライダーAとコライダーBの距離を計算
	float distance = Mathf::Length(sphereA.center - sphereB.center);
	//球と球の交差判定
	if (distance <= sphereA.radius + sphereB.radius)
	{
		return true;
	}
	return false;
}

bool CollisionManager::CheckCollisionSphereAABB(const Sphere& sphere, const AABB& aabb)
{
	//最近接点を求める
	Vector3 closestPoint{
		std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),
		std::clamp(sphere.center.y,aabb.min.y,aabb.max.y),
		std::clamp(sphere.center.z,aabb.min.z,aabb.max.z) };
	//最近接点と球の中心との距離を求める
	float distance = Mathf::Length(closestPoint - sphere.center);
	//距離が半径よりも小さければ衝突
	if (distance <= sphere.radius)
	{
		return true;
	}
	return false;
}

bool CollisionManager::CheckCollisionAABB(const AABB& aabbA, const AABB& aabbB)
{
	if (aabbA.min.x <= aabbB.max.x && aabbA.max.x >= aabbB.min.x &&
		aabbA.min.y <= aabbB.max.y && aabbA.max.y >= aabbB.min.y &&
		aabbA.min.z <= aabbB.max.z && aabbA.max.z >= aabbB.min.z)
	{
		return true;
	}
	return false;
}

bool CollisionManager::CheckCollisionAABBOBB(const AABB& aabb, const OBB& obb)
{
	Vector3 aabbCenter = (aabb.min + aabb.max) * 0.5f;

	Vector3 aabbExtent = {
		0.5f * (aabb.max.x - aabb.min.x),
		0.5f * (aabb.max.y - aabb.min.y),
		0.5f * (aabb.max.z - aabb.min.z),
	};

	Vector3 separationVector = aabbCenter - obb.center;

	Vector3 aabbAxis1 = { 1.0f, 0.0f, 0.0f };
	Vector3 aabbEdge1 = aabbAxis1 * aabbExtent.x;
	Vector3 aabbAxis2 = { 0.0f, 1.0f, 0.0f };
	Vector3 aabbEdge2 = aabbAxis2 * aabbExtent.y;
	Vector3 aabbAxis3 = { 0.0f, 0.0f, 1.0f };
	Vector3 aabbEdge3 = aabbAxis3 * aabbExtent.z;

	Vector3 obbAxis1 = obb.orientations[0];
	Vector3 obbEdge1 = obbAxis1 * obb.size.x;
	Vector3 obbAxis2 = obb.orientations[1];
	Vector3 obbEdge2 = obbAxis2 * obb.size.y;
	Vector3 obbAxis3 = obb.orientations[2];
	Vector3 obbEdge3 = obbAxis3 * obb.size.z;

	//aabbAxis1
	float aabbProjectionExtent = Mathf::Length(aabbEdge1);
	float obbProjectionExtent = LenSegOnSeparateAxis(&aabbAxis1, &obbEdge1, &obbEdge2, &obbEdge3);
	float projectionOverlap = fabs(Mathf::Dot(separationVector, aabbAxis1));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	//aabbAxis2
	aabbProjectionExtent = Mathf::Length(aabbEdge2);
	obbProjectionExtent = LenSegOnSeparateAxis(&aabbAxis2, &obbEdge1, &obbEdge2, &obbEdge3);
	projectionOverlap = fabs(Mathf::Dot(separationVector, aabbAxis2));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	//aabbAxis3
	aabbProjectionExtent = Mathf::Length(aabbEdge3);
	obbProjectionExtent = LenSegOnSeparateAxis(&aabbAxis3, &obbEdge1, &obbEdge2, &obbEdge3);
	projectionOverlap = fabs(Mathf::Dot(separationVector, aabbAxis3));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	//obbAxis1
	aabbProjectionExtent = LenSegOnSeparateAxis(&obbAxis1, &aabbEdge1, &aabbEdge2, &aabbEdge3);
	obbProjectionExtent = Mathf::Length(obbEdge1);
	projectionOverlap = fabs(Mathf::Dot(separationVector, obbAxis1));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	//obbAxis2
	aabbProjectionExtent = LenSegOnSeparateAxis(&obbAxis2, &aabbEdge1, &aabbEdge2, &aabbEdge3);
	obbProjectionExtent = Mathf::Length(obbEdge2);
	projectionOverlap = fabs(Mathf::Dot(separationVector, obbAxis2));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	//obbAxis3
	aabbProjectionExtent = LenSegOnSeparateAxis(&obbAxis3, &aabbEdge1, &aabbEdge2, &aabbEdge3);
	obbProjectionExtent = Mathf::Length(obbEdge3);
	projectionOverlap = fabs(Mathf::Dot(separationVector, obbAxis3));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	//Cross(aabbAxis1, obbAxis1)
	Vector3 crossProduct = Mathf::Cross(aabbAxis1, obbAxis1);
	aabbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &aabbEdge2, &aabbEdge3, 0);
	obbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &obbEdge2, &obbEdge3, 0);
	projectionOverlap = fabs(Mathf::Dot(separationVector, crossProduct));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	//Cross(aabbAxis1, obbAxis2)
	crossProduct = Mathf::Cross(aabbAxis1, obbAxis2);
	aabbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &aabbEdge2, &aabbEdge3, 0);
	obbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &obbEdge1, &obbEdge3, 0);
	projectionOverlap = fabs(Mathf::Dot(separationVector, crossProduct));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	//Cross(aabbAxis1, obbAxis3)
	crossProduct = Mathf::Cross(aabbAxis1, obbAxis3);
	aabbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &aabbEdge2, &aabbEdge3, 0);
	obbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &obbEdge1, &obbEdge2, 0);
	projectionOverlap = fabs(Mathf::Dot(separationVector, crossProduct));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	//Cross(aabbAxis2, obbAxis1)
	crossProduct = Mathf::Cross(aabbAxis2, obbAxis1);
	aabbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &aabbEdge1, &aabbEdge3, 0);
	obbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &obbEdge2, &obbEdge3, 0);
	projectionOverlap = fabs(Mathf::Dot(separationVector, crossProduct));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	//Cross(aabbAxis2, obbAxis2)
	crossProduct = Mathf::Cross(aabbAxis2, obbAxis2);
	aabbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &aabbEdge1, &aabbEdge3, 0);
	obbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &obbEdge1, &obbEdge3, 0);
	projectionOverlap = fabs(Mathf::Dot(separationVector, crossProduct));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	//Cross(aabbAxis2, obbAxis3)
	crossProduct = Mathf::Cross(aabbAxis2, obbAxis3);
	aabbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &aabbEdge1, &aabbEdge3, 0);
	obbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &obbEdge1, &obbEdge2, 0);
	projectionOverlap = fabs(Mathf::Dot(separationVector, crossProduct));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	//Cross(aabbAxis3, obbAxis1)
	crossProduct = Mathf::Cross(aabbAxis3, obbAxis1);
	aabbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &aabbEdge1, &aabbEdge2, 0);
	obbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &obbEdge2, &obbEdge3, 0);
	projectionOverlap = fabs(Mathf::Dot(separationVector, crossProduct));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	//Cross(aabbAxis3, obbAxis2)
	crossProduct = Mathf::Cross(aabbAxis3, obbAxis2);
	aabbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &aabbEdge1, &aabbEdge2, 0);
	obbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &obbEdge1, &obbEdge3, 0);
	projectionOverlap = fabs(Mathf::Dot(separationVector, crossProduct));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	//Cross(aabbAxis3, obbAxis3)
	crossProduct = Mathf::Cross(aabbAxis3, obbAxis3);
	aabbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &aabbEdge1, &aabbEdge2, 0);
	obbProjectionExtent = LenSegOnSeparateAxis(&crossProduct, &obbEdge1, &obbEdge2, 0);
	projectionOverlap = fabs(Mathf::Dot(separationVector, crossProduct));
	if (projectionOverlap > aabbProjectionExtent + obbProjectionExtent)
	{
		return false;
	}

	return true;
}

float CollisionManager::LenSegOnSeparateAxis(Vector3* Sep, Vector3* e1, Vector3* e2, Vector3* e3)
{
	float r1 = fabs(Mathf::Dot(*Sep, *e1));
	float r2 = fabs(Mathf::Dot(*Sep, *e2));
	float r3 = e3 ? (fabs(Mathf::Dot(*Sep, *e3))) : 0;
	return r1 + r2 + r3;
}
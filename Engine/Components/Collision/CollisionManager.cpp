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

	float aabbHalfSize[3] = {
	0.5f * (aabb.max.x - aabb.min.x),
	0.5f * (aabb.max.y - aabb.min.y),
	0.5f * (aabb.max.z - aabb.min.z),
	};

	Vector3 aabbAxis[3] = {
		{1.0f,0.0f,0.0f},
		{0.0f,1.0f,0.0f},
		{0.0f,0.0f,1.0f}
	};

	float obbHalfSize[3] = {
		obb.size.x,
		obb.size.y,
		obb.size.z
	};

	Vector3 obbAxis[3] = {
		obb.orientations[0],
		obb.orientations[1],
		obb.orientations[2],
	};

	float t[3] = {
		obb.center.x - aabbCenter.x,
		obb.center.y - aabbCenter.y,
		obb.center.z - aabbCenter.z,
	};

	const float EPSILON = 1.175494e-37f;

	float R[3][3], AbsR[3][3];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			R[i][j] = Mathf::Dot(aabbAxis[i], obb.orientations[j]);
			AbsR[i][j] = fabsf(R[i][j]) + EPSILON;
		}
	}

	//軸L=A0, L=A1, L=A2判定
	float ra, rb;

	for (int i = 0; i < 3; i++)
	{
		ra = aabbHalfSize[i];
		rb = obbHalfSize[0] * AbsR[i][0] + obbHalfSize[1] * AbsR[i][1] + obbHalfSize[2] * AbsR[i][2];
		if (fabsf(t[i]) > ra + rb)return false;
	}
	//軸L=B0, L=B1, L=B2判定
	for (int i = 0; i < 3; i++)
	{
		ra = aabbHalfSize[0] * AbsR[0][i] + aabbHalfSize[1] * AbsR[1][i] + aabbHalfSize[2] * AbsR[2][i];
		rb = obbHalfSize[i];
		if (fabsf(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb)return false;
	}

	//軸L=A0 X B0判定
	ra = aabbHalfSize[1] * AbsR[2][0] + aabbHalfSize[2] * AbsR[1][0];
	rb = obbHalfSize[1] * AbsR[0][2] + obbHalfSize[2] * AbsR[0][1];
	if (fabsf(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb)return false;

	//軸L=A0 X B1判定
	ra = aabbHalfSize[1] * AbsR[2][1] + aabbHalfSize[2] * AbsR[1][1];
	rb = obbHalfSize[0] * AbsR[0][2] + obbHalfSize[2] * AbsR[0][0];
	if (fabsf(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb)return false;

	//軸L=A0 X B2判定
	ra = aabbHalfSize[1] * AbsR[2][2] + aabbHalfSize[2] * AbsR[1][2];
	rb = obbHalfSize[0] * AbsR[0][1] + obbHalfSize[1] * AbsR[0][0];
	if (fabsf(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb)return false;

	//軸L=A1 X B0判定
	ra = aabbHalfSize[0] * AbsR[2][0] + aabbHalfSize[2] * AbsR[0][0];
	rb = obbHalfSize[1] * AbsR[1][2] + obbHalfSize[2] * AbsR[1][1];
	if (fabsf(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb)return false;

	//軸L=A1 X B1判定
	ra = aabbHalfSize[0] * AbsR[2][1] + aabbHalfSize[2] * AbsR[0][1];
	rb = obbHalfSize[0] * AbsR[1][2] + obbHalfSize[2] * AbsR[1][0];
	if (fabsf(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb)return false;

	//軸L=A1 X B2判定
	ra = aabbHalfSize[0] * AbsR[2][2] + aabbHalfSize[2] * AbsR[0][2];
	rb = obbHalfSize[0] * AbsR[1][1] + obbHalfSize[1] * AbsR[1][0];
	if (fabsf(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb)return false;

	//軸L=A2 X B0判定
	ra = aabbHalfSize[0] * AbsR[1][0] + aabbHalfSize[1] * AbsR[0][0];
	rb = obbHalfSize[1] * AbsR[2][2] + obbHalfSize[2] * AbsR[2][1];
	if (fabsf(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb)return false;

	//軸L=A2 X B1判定
	ra = aabbHalfSize[0] * AbsR[1][1] + aabbHalfSize[1] * AbsR[0][1];
	rb = obbHalfSize[0] * AbsR[2][2] + obbHalfSize[2] * AbsR[2][0];
	if (fabsf(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb)return false;

	//軸L=A2 X B2判定
	ra = aabbHalfSize[0] * AbsR[1][2] + aabbHalfSize[1] * AbsR[0][2];
	rb = obbHalfSize[0] * AbsR[2][1] + obbHalfSize[1] * AbsR[2][0];
	if (fabsf(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb)return false;

	return true;
}
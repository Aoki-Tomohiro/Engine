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
		if (colliderA->GetCollisionAttribute() & kCollisionPrimitiveAABB)
		{
			AABB aabb = {
				.min{colliderA->GetWorldPosition().x + colliderA->GetAABB().min.x,colliderA->GetWorldPosition().y + colliderA->GetAABB().min.y,colliderA->GetWorldPosition().z + colliderA->GetAABB().min.z},
				.max{colliderA->GetWorldPosition().x + colliderA->GetAABB().max.x,colliderA->GetWorldPosition().y + colliderA->GetAABB().max.y,colliderA->GetWorldPosition().z + colliderA->GetAABB().max.z},
			};

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

			OBB obb = colliderB->GetOBB();

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
				obb.center.x - colliderA->GetWorldPosition().x,
				obb.center.y - colliderA->GetWorldPosition().y,
				obb.center.z - colliderA->GetWorldPosition().z,
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
				if (fabsf(t[i]) > ra + rb)return;
			}
			//軸L=B0, L=B1, L=B2判定
			for (int i = 0; i < 3; i++)
			{
				ra = aabbHalfSize[0] * AbsR[0][i] + aabbHalfSize[1] * AbsR[1][i] + aabbHalfSize[2] * AbsR[2][i];
				rb = obbHalfSize[i];
				if (fabsf(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb)return;
			}

			//軸L=A0 X B0判定
			ra = aabbHalfSize[1] * AbsR[2][0] + aabbHalfSize[2] * AbsR[1][0];
			rb = obbHalfSize[1] * AbsR[0][2] + obbHalfSize[2] * AbsR[0][1];
			if (fabsf(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb)return;

			//軸L=A0 X B1判定
			ra = aabbHalfSize[1] * AbsR[2][1] + aabbHalfSize[2] * AbsR[1][1];
			rb = obbHalfSize[0] * AbsR[0][2] + obbHalfSize[2] * AbsR[0][0];
			if (fabsf(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb)return;

			//軸L=A0 X B2判定
			ra = aabbHalfSize[1] * AbsR[2][2] + aabbHalfSize[2] * AbsR[1][2];
			rb = obbHalfSize[0] * AbsR[0][1] + obbHalfSize[1] * AbsR[0][0];
			if (fabsf(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb)return;

			//軸L=A1 X B0判定
			ra = aabbHalfSize[0] * AbsR[2][0] + aabbHalfSize[2] * AbsR[0][0];
			rb = obbHalfSize[1] * AbsR[1][2] + obbHalfSize[2] * AbsR[1][1];
			if (fabsf(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb)return;

			//軸L=A1 X B1判定
			ra = aabbHalfSize[0] * AbsR[2][1] + aabbHalfSize[2] * AbsR[0][1];
			rb = obbHalfSize[0] * AbsR[1][2] + obbHalfSize[2] * AbsR[1][0];
			if (fabsf(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb)return;

			//軸L=A1 X B2判定
			ra = aabbHalfSize[0] * AbsR[2][2] + aabbHalfSize[2] * AbsR[0][2];
			rb = obbHalfSize[0] * AbsR[1][1] + obbHalfSize[1] * AbsR[1][0];
			if (fabsf(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb)return;

			//軸L=A2 X B0判定
			ra = aabbHalfSize[0] * AbsR[1][0] + aabbHalfSize[1] * AbsR[0][0];
			rb = obbHalfSize[1] * AbsR[2][2] + obbHalfSize[2] * AbsR[2][1];
			if (fabsf(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb)return;

			//軸L=A2 X B1判定
			ra = aabbHalfSize[0] * AbsR[1][1] + aabbHalfSize[1] * AbsR[0][1];
			rb = obbHalfSize[0] * AbsR[2][2] + obbHalfSize[2] * AbsR[2][0];
			if (fabsf(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb)return;

			//軸L=A2 X B2判定
			ra = aabbHalfSize[0] * AbsR[1][2] + aabbHalfSize[1] * AbsR[0][2];
			rb = obbHalfSize[0] * AbsR[2][1] + obbHalfSize[1] * AbsR[2][0];
			if (fabsf(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb)return;

			//コライダーAの衝突時コールバックを呼び出す
			colliderA->OnCollision(colliderB);
			//コライダーBの衝突時コールバックを呼び出す
			colliderB->OnCollision(colliderA);
		}
		//ColliderBがAABBの場合
		else if (colliderB->GetCollisionAttribute() & kCollisionPrimitiveAABB)
		{
			AABB aabb = {
				.min{colliderB->GetWorldPosition().x + colliderB->GetAABB().min.x,colliderB->GetWorldPosition().y + colliderB->GetAABB().min.y,colliderB->GetWorldPosition().z + colliderB->GetAABB().min.z},
				.max{colliderB->GetWorldPosition().x + colliderB->GetAABB().max.x,colliderB->GetWorldPosition().y + colliderB->GetAABB().max.y,colliderB->GetWorldPosition().z + colliderB->GetAABB().max.z},
			};

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

			OBB obb = colliderA->GetOBB();

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
				obb.center.x - colliderB->GetWorldPosition().x,
				obb.center.y - colliderB->GetWorldPosition().y,
				obb.center.z - colliderB->GetWorldPosition().z,
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
				if (fabsf(t[i]) > ra + rb)return;
			}
			//軸L=B0, L=B1, L=B2判定
			for (int i = 0; i < 3; i++)
			{
				ra = aabbHalfSize[0] * AbsR[0][i] + aabbHalfSize[1] * AbsR[1][i] + aabbHalfSize[2] * AbsR[2][i];
				rb = obbHalfSize[i];
				if (fabsf(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb)return;
			}

			//軸L=A0 X B0判定
			ra = aabbHalfSize[1] * AbsR[2][0] + aabbHalfSize[2] * AbsR[1][0];
			rb = obbHalfSize[1] * AbsR[0][2] + obbHalfSize[2] * AbsR[0][1];
			if (fabsf(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb)return;

			//軸L=A0 X B1判定
			ra = aabbHalfSize[1] * AbsR[2][1] + aabbHalfSize[2] * AbsR[1][1];
			rb = obbHalfSize[0] * AbsR[0][2] + obbHalfSize[2] * AbsR[0][0];
			if (fabsf(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb)return;

			//軸L=A0 X B2判定
			ra = aabbHalfSize[1] * AbsR[2][2] + aabbHalfSize[2] * AbsR[1][2];
			rb = obbHalfSize[0] * AbsR[0][1] + obbHalfSize[1] * AbsR[0][0];
			if (fabsf(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb)return;

			//軸L=A1 X B0判定
			ra = aabbHalfSize[0] * AbsR[2][0] + aabbHalfSize[2] * AbsR[0][0];
			rb = obbHalfSize[1] * AbsR[1][2] + obbHalfSize[2] * AbsR[1][1];
			if (fabsf(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb)return;

			//軸L=A1 X B1判定
			ra = aabbHalfSize[0] * AbsR[2][1] + aabbHalfSize[2] * AbsR[0][1];
			rb = obbHalfSize[0] * AbsR[1][2] + obbHalfSize[2] * AbsR[1][0];
			if (fabsf(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb)return;

			//軸L=A1 X B2判定
			ra = aabbHalfSize[0] * AbsR[2][2] + aabbHalfSize[2] * AbsR[0][2];
			rb = obbHalfSize[0] * AbsR[1][1] + obbHalfSize[1] * AbsR[1][0];
			if (fabsf(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb)return;

			//軸L=A2 X B0判定
			ra = aabbHalfSize[0] * AbsR[1][0] + aabbHalfSize[1] * AbsR[0][0];
			rb = obbHalfSize[1] * AbsR[2][2] + obbHalfSize[2] * AbsR[2][1];
			if (fabsf(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb)return;

			//軸L=A2 X B1判定
			ra = aabbHalfSize[0] * AbsR[1][1] + aabbHalfSize[1] * AbsR[0][1];
			rb = obbHalfSize[0] * AbsR[2][2] + obbHalfSize[2] * AbsR[2][0];
			if (fabsf(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb)return;

			//軸L=A2 X B2判定
			ra = aabbHalfSize[0] * AbsR[1][2] + aabbHalfSize[1] * AbsR[0][2];
			rb = obbHalfSize[0] * AbsR[2][1] + obbHalfSize[1] * AbsR[2][0];
			if (fabsf(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb)return;

			//コライダーAの衝突時コールバックを呼び出す
			colliderA->OnCollision(colliderB);
			//コライダーBの衝突時コールバックを呼び出す
			colliderB->OnCollision(colliderA);
		}
	}
}
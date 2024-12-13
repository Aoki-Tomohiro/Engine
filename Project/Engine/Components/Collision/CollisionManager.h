/**
 * @file CollisionManager.h
 * @brief 衝突判定を行うファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "OBBCollider.h"
#include <list>

class CollisionManager
{
public:
	/// <summary>
	/// コライダーのリストをクリア
	/// </summary>
	void ClearColliderList();

	/// <summary>
	/// コライダーのリストに追加
	/// </summary>
	/// <param name="collider">コライダー</param>
	void SetColliderList(Collider* collider);

	/// <summary>
	/// 全てのコライダーの衝突判定を行う
	/// </summary>
	void CheckAllCollisions();

private:
	/// <summary>
	/// コライダーのペアを確認
	/// </summary>
	/// <param name="colliderA">コライダー1</param>
	/// <param name="colliderB">コライダー2</param>
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	/// <summary>
	/// 球と球の衝突判定
	/// </summary>
	/// <param name="sphere1">球1</param>
	/// <param name="sphere2">球2</param>
	/// <returns>衝突しているかどうか</returns>
	bool CheckSphereSphereCollision(const SphereCollider* sphere1, const SphereCollider* sphere2);
	
	/// <summary>
	/// AABBとAABBの衝突判定
	/// </summary>
	/// <param name="aabb1">AABB1</param>
	/// <param name="aabb2">AABB2</param>
	/// <returns>衝突しているかどうか</returns>
	bool CheckAABBAABBCollision(const AABBCollider* aabb1, const AABBCollider* aabb2);

	/// <summary>
	/// OBBとOBBの衝突判定
	/// </summary>
	/// <param name="obb1">OBB1</param>
	/// <param name="obb2">OBB2</param>
	/// <returns>衝突しているかどうか</returns>
	bool CheckOBBOBBCollision(const OBBCollider* obb1, const OBBCollider* obb2);

	/// <summary>
	/// 球とAABBの衝突判定
	/// </summary>
	/// <param name="sphere1">球</param>
	/// <param name="aabb1">AABB</param>
	/// <returns>衝突しているかどうか</returns>
	bool CheckSphereAABBCollision(const SphereCollider* sphere1, const AABBCollider* aabb1);
	
	/// <summary>
	/// 球とOBBの衝突判定
	/// </summary>
	/// <param name="sphere1">球</param>
	/// <param name="obb1">OBB</param>
	/// <returns>衝突しているかどうか</returns>
	bool CheckSphereOBBCollision(const SphereCollider* sphere1, const OBBCollider* obb1);

	/// <summary>
	/// AABBとOBBの衝突判定
	/// </summary>
	/// <param name="aabb1">AABB</param>
	/// <param name="obb1">OBB</param>
	/// <returns>衝突しているかどうか</returns>
	bool CheckAABBOBBCollision(const AABBCollider* aabb1, const OBBCollider* obb1);

private:
	std::list<Collider*> colliders_{};
};


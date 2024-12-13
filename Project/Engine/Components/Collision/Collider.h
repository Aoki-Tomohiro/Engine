/**
 * @file Collider.h
 * @brief コライダーを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Components/Base/RenderComponent.h"
#include <cstdint>
#include <set>

class Collider : public RenderComponent
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Collider() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera& camera) override;

	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="other">衝突相手</param>
	void OnCollision(GameObject* other);

	/// <summary>
	/// 衝突処理（当たった瞬間）
	/// </summary>
	/// <param name="other">衝突相手</param>
	void OnCollisionEnter(GameObject* other);

	/// <summary>
	/// 衝突処理（離れた瞬間）
	/// </summary>
	/// <param name="other">衝突相手</param>
	void OnCollisionExit(GameObject* other);

	/// <summary>
	/// 現在衝突しているコライダーを追加
	/// </summary>
	/// <param name="collider">コライダー</param>
	void AddCollision(Collider* collider) { currentCollisions_.insert(collider); };

	/// <summary>
	/// 現在衝突しているコライダーを削除
	/// </summary>
	/// <param name="collider">コライダー</param>
	void RemoveCollision(Collider* collider) { currentCollisions_.erase(collider); };

	/// <summary>
	/// 同じ相手と衝突しているか確認
	/// </summary>
	/// <param name="collider">衝突相手</param>
	/// <returns>同じ相手と衝突しているか</returns>
	const bool IsCollidingWith(Collider* collider) const { return currentCollisions_.find(collider) != currentCollisions_.end(); };

	//衝突属性を取得・設定
	const uint32_t GetCollisionAttribute() const { return collisionAttribute_; };
	void SetCollisionAttribute(uint32_t collisionAttribute) { collisionAttribute_ = collisionAttribute; };

	//衝突マスクを取得・設定
	const uint32_t GetCollisionMask() const { return collisionMask_; };
	void SetCollisionMask(uint32_t collisionMask) { collisionMask_ = collisionMask; };

	//コライダーを有効にするかを取得・設定
	const bool GetCollisionEnabled() const { return collisionEnabled_; };
	void SetCollisionEnabled(const bool collisionEnabled) { collisionEnabled_ = collisionEnabled; };

	//デバッグ描画をするかどうかを設定
	void SetDebugDrawEnabled(const bool debugDrawEnabled) { debugDrawEnabled_ = debugDrawEnabled; };

protected:
	std::set<Collider*> currentCollisions_;

	uint32_t collisionAttribute_ = 0xffffffff;

	uint32_t collisionMask_ = 0xffffffff;

	bool collisionEnabled_ = true;

	bool debugDrawEnabled_ = false;
};


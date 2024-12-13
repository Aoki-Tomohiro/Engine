/**
 * @file SphereCollider.h
 * @brief Sphereコライダーを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Collider.h"
#include "Engine/Math/Vector3.h"

class SphereCollider : public Collider
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override {};

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera& camera) override;

	//ワールド座標系の中心点を取得・設定
	const Vector3& GetWorldCenter() const { return worldCenter_; };
	void SetWorldCenter(const Vector3& worldCenter)
	{
		worldCenter_ = worldCenter;
		isWorldCenterSet_ = true;
	};

	//ワールド座標系の中心点をクリア
	void ClearWorldCenter() { isWorldCenterSet_ = false; };

	//中心点を取得・設定
	const Vector3& GetCenter() const { return center_; };
	void SetCenter(const Vector3& center) { center_ = center; };

	//半径を取得・設定
	const float GetRadius() const { return radius_; };
	void SetRadius(const float radius) { radius_ = radius; };

private:
	Vector3 worldCenter_{};

	Vector3 center_{};

	float radius_ = 1.0f;

	bool isWorldCenterSet_ = false;
};


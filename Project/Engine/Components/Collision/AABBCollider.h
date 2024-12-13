/**
 * @file AABBCollider.h
 * @brief AABBコライダーを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Collider.h"
#include "Engine/Math/Vector3.h"

class AABBCollider : public Collider
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

	//最小値を取得・設定
	const Vector3& GetMin() const { return min_; };
	void SetMin(const Vector3& min) { min_ = min; };

	//最大値を取得・設定
	const Vector3& GetMax() const { return max_; };
	void SetMax(const Vector3& max) { max_ = max; };

private:
	Vector3 worldCenter_{};

	Vector3 center_{};

	Vector3 min_{ -1.0f,-1.0f,-1.0f };

	Vector3 max_{ 1.0f,1.0f,1.0f };

	bool isWorldCenterSet_ = false;
};


/**
 * @file OBBCollider.h
 * @brief OBBコライダーを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Collider.h"
#include "Engine/Math/Vector3.h"

class OBBCollider : public Collider
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

	//軸を取得・設定
    const Vector3& GetOrientation(size_t index) const
    {
        if (index < 3)
        {
            return orientations_[index];
        }
        static Vector3 defaultVector;
        return defaultVector;
    }
    void SetOrientations(const Vector3& orientation1, const Vector3& orientation2, const Vector3& orientation3)
    {
        orientations_[0] = orientation1;
        orientations_[1] = orientation2;
        orientations_[2] = orientation3;
    }

	//サイズを取得・設定
    const Vector3& GetSize() const { return size_; };
    void SetSize(const Vector3& size) { size_ = size; };

private:
    //ワールド座標の中心点
    Vector3 worldCenter_{};

    //中心点
	Vector3 center_ = { 0.0f,0.0f,0.0f };

    //座標軸。正規化・直交必須
	Vector3 orientations_[3] = { {1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f} };

    //座標軸方向の長さの半分。中心から面までの距離
	Vector3 size_ = { 1.0f,1.0f,1.0f };

    //ワールド座標系の中心座標が設定されているかどうか
    bool isWorldCenterSet_ = false;
};


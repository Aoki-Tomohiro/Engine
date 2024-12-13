/**
 * @file GravityField.h
 * @brief 重力フィールドを管理するクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Math/Vector3.h"
#include <string>

class GravityField
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="name">名前</param>
	/// <param name="lifeTime">寿命</param>
	void Initialize(const std::string& name, const float lifeTime);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

    //名前を取得・設定
    const std::string& GetName() const { return name_; }
    void SetName(const std::string& name) { name_ = name; }

    //座標を取得・設定
    const Vector3& GetTranslate() const { return translate_; }
    void SetTranslate(const Vector3& translate) { translate_ = translate; }

    //最小値を取得・設定
    const Vector3& GetMin() const { return min_; }
    void SetMin(const Vector3& min) { min_ = min; }

    //最大値を取得・設定
    const Vector3& GetMax() const { return max_; }
    void SetMax(const Vector3& max) { max_ = max; }

    //強さを取得・設定
    const float GetStrength() const { return strength_; }
    void SetStrength(float strength) { strength_ = strength; }

    //止める距離を取得・設定
    const float GetStopDistance() const { return stopDistance_; }
    void SetStopDistance(float stopDistance) { stopDistance_ = stopDistance; }

    //死亡フラグを取得・設定
    const bool GetIsDead() const { return isDead_; };
    void SetIsDead(const bool isDead) { isDead_ = isDead; };

private:
    //名前
	std::string name_;

    //位置
    Vector3 translate_ = { 0.0f,0.0f,0.0f };

    //最小範囲
    Vector3 min_ = { -1.0f,-1.0f,-1.0f };

    //最大範囲
    Vector3 max_ = { 1.0f,1.0f,1.0f };

    //重力の強さ
    float strength_ = 0.06f;

    //動きを止める中心点からの距離
    float stopDistance_ = 0.4f;

    //死亡フラグ
    bool isDead_ = false;

    //死亡までのタイマー
    float deathTimer_ = 0.0f;
};


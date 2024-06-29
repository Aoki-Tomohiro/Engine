#pragma once
#include "Engine/Math/Vector3.h"
#include <string>

class AccelerationField
{
public:
    void Initialize(const std::string& name, const float lifeTime);

	void Update();

    const std::string& GetName() const { return name_; }

    const Vector3& GetAcceleration() const { return acceleration_; }

    void SetAcceleration(const Vector3& acceleration) { acceleration_ = acceleration; }

    const Vector3& GetTranslate() const { return translate_; }

    void SetTranslate(const Vector3& translate) { translate_ = translate; }

    const Vector3& GetMin() const { return min_; }

    void SetMin(const Vector3& min) { min_ = min; }

    const Vector3& GetMax() const { return max_; }

    void SetMax(const Vector3& max) { max_ = max; }

    const bool GetIsDead() const { return isDead_; };

    void SetIsDead(const bool isDead) { isDead_ = isDead; };

private:
    //名前
	std::string name_{};

    //加速度
    Vector3 acceleration_ = { 0.0f,-0.08f,0.0f };

    //位置
    Vector3 translate_ = { 0.0f,0.0f,0.0f };

    //最小範囲
    Vector3 min_ = { -1.0f,-1.0f,-1.0f };

    //最大範囲
    Vector3 max_ = { 1.0f,1.0f,1.0f };

    //死亡フラグ
    bool isDead_ = false;

    //死亡までのタイマー
    float deathTimer_ = 0.0f;
};


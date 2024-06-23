#pragma once
#include "Collider.h"
#include "Engine/Math/Vector3.h"

class OBBCollider : public Collider
{
public:
	void Initialize() override;

	void Update() override;

    void Draw(const Camera& camera) override;

	const Vector3& GetCenter() const { return center_; };

	void SetCenter(const Vector3& center) { center_ = center; };

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

    const Vector3& GetSize() const { return size_; };

    void SetSize(const Vector3& size) { size_ = size; };

private:
    //中心点
	Vector3 center_ = { 0.0f,0.0f,0.0f };

    //座標軸。正規化・直交必須
	Vector3 orientations_[3] = { {1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f} };

    //座標軸方向の長さの半分。中心から面までの距離
	Vector3 size_ = { 1.0f,1.0f,1.0f };
};


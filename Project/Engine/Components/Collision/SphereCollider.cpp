#include "SphereCollider.h"
#include "Engine/3D/Primitive/LineRenderer.h"
#include <numbers>

void SphereCollider::Initialize()
{

}

void SphereCollider::Update()
{

}

void SphereCollider::Draw(const Camera& camera)
{
    if (debugDrawEnabled_)
    {
        //球の作成
        const uint32_t kSubdivision = 8;
        uint32_t latIndex = 0;
        uint32_t lonIndex = 0;
        //経度分割一つ分の角度φd
        const float kLonEvery = std::numbers::pi_v<float> *2.0f / float(kSubdivision);
        //緯度分割一つ分の角度θd
        const float kLatEvery = std::numbers::pi_v<float> / float(kSubdivision);
        //緯度の方向に分割
        for (latIndex = 0; latIndex < kSubdivision; ++latIndex) {
            float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex;//θ
            //経度の方向に分割しながら線を描く
            for (lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
                uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;
                float lon = lonIndex * kLonEvery;//φ
                //基準点a
                Vector3 pointA = {
                    center_.x + radius_ * std::cos(lat) * std::cos(lon),
                    center_.y + radius_ * std::sin(lat),
                    center_.z + radius_ * std::cos(lat) * std::sin(lon)
                };

                //点b
                Vector3 pointB = {
                    center_.x + radius_ * std::cos(lat + kLatEvery) * std::cos(lon),
                    center_.y + radius_ * std::sin(lat + kLatEvery),
                    center_.z + radius_ * std::cos(lat + kLatEvery) * std::sin(lon)
                };

                //点c
                Vector3 pointC = {
                    center_.x + radius_ * std::cos(lat) * std::cos(lon + kLonEvery),
                    center_.y + radius_ * std::sin(lat),
                    center_.z + radius_ * std::cos(lat) * std::sin(lon + kLonEvery)
                };

                //点d
                Vector3 pointD = {
                    center_.x + radius_ * std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery),
                    center_.y + radius_ * std::sin(lat + kLatEvery),
                    center_.z + radius_ * std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery)
                };

                //各頂点間の線分を追加
                LineRenderer* lineRenderer = LineRenderer::GetInstance();
                lineRenderer->AddLine(pointA, pointB);
                lineRenderer->AddLine(pointA, pointC);
                lineRenderer->AddLine(pointB, pointD);
                lineRenderer->AddLine(pointC, pointD);
            }
        }

        //Colliderの描画
        Collider::Draw(camera);
    }
}
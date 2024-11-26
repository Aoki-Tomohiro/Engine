#include "SphereCollider.h"
#include "Engine/3D/Primitive/LineRenderer.h"
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include <numbers>

void SphereCollider::Initialize()
{

}

void SphereCollider::Update()
{
    if (!isWorldCenterSet_)
    {
        //トランスフォームコンポーネントからワールド座標を計算
        TransformComponent* transformComponent = owner_->GetComponent<TransformComponent>();
        worldCenter_ = transformComponent->GetWorldPosition() + center_;
    }
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
                //φ
                float lon = lonIndex * kLonEvery;
                //基準点a
                Vector3 pointA = {
                    worldCenter_.x + radius_ * std::cos(lat) * std::cos(lon),
                    worldCenter_.y + radius_ * std::sin(lat),
                    worldCenter_.z + radius_ * std::cos(lat) * std::sin(lon)
                };

                //点b
                Vector3 pointB = {
                    worldCenter_.x + radius_ * std::cos(lat + kLatEvery) * std::cos(lon),
                    worldCenter_.y + radius_ * std::sin(lat + kLatEvery),
                    worldCenter_.z + radius_ * std::cos(lat + kLatEvery) * std::sin(lon)
                };

                //点c
                Vector3 pointC = {
                    worldCenter_.x + radius_ * std::cos(lat) * std::cos(lon + kLonEvery),
                    worldCenter_.y + radius_ * std::sin(lat),
                    worldCenter_.z + radius_ * std::cos(lat) * std::sin(lon + kLonEvery)
                };

                //点d
                Vector3 pointD = {
                    worldCenter_.x + radius_ * std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery),
                    worldCenter_.y + radius_ * std::sin(lat + kLatEvery),
                    worldCenter_.z + radius_ * std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery)
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
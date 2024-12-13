/**
 * @file AABBCollider.cpp
 * @brief AABBコライダーを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "AABBCollider.h"
#include "Engine/3D/Primitive/LineRenderer.h"
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Transform/TransformComponent.h"

void AABBCollider::Update()
{
	if (!isWorldCenterSet_)
	{
		//トランスフォームコンポーネントからワールド座標を計算
		TransformComponent* transformComponent = owner_->GetComponent<TransformComponent>();
		worldCenter_ = transformComponent->GetWorldPosition() + center_;
	}
}

void AABBCollider::Draw(const Camera& camera)
{
	if (debugDrawEnabled_)
	{
		//頂点データ
		std::vector<Vector3> corners(8);

		//AABBの8つの頂点を計算する
		corners[0] = worldCenter_ + min_;
		corners[1] = Vector3(worldCenter_.x + max_.x, worldCenter_.y + min_.y, worldCenter_.z + min_.z);
		corners[2] = Vector3(worldCenter_.x + min_.x, worldCenter_.y + max_.y, worldCenter_.z + min_.z);
		corners[3] = Vector3(worldCenter_.x + max_.x, worldCenter_.y + max_.y, worldCenter_.z + min_.z);
		corners[4] = Vector3(worldCenter_.x + min_.x, worldCenter_.y + min_.y, worldCenter_.z + max_.z);
		corners[5] = Vector3(worldCenter_.x + max_.x, worldCenter_.y + min_.y, worldCenter_.z + max_.z);
		corners[6] = Vector3(worldCenter_.x + min_.x, worldCenter_.y + max_.y, worldCenter_.z + max_.z);
		corners[7] = worldCenter_ + max_;

		//各頂点を線分で描画する
		LineRenderer* lineRenderer = LineRenderer::GetInstance();
		lineRenderer->AddLine(corners[0], corners[1]);
		lineRenderer->AddLine(corners[0], corners[2]);
		lineRenderer->AddLine(corners[0], corners[4]);
		lineRenderer->AddLine(corners[1], corners[3]);
		lineRenderer->AddLine(corners[1], corners[5]);
		lineRenderer->AddLine(corners[2], corners[3]);
		lineRenderer->AddLine(corners[2], corners[6]);
		lineRenderer->AddLine(corners[3], corners[7]);
		lineRenderer->AddLine(corners[4], corners[5]);
		lineRenderer->AddLine(corners[4], corners[6]);
		lineRenderer->AddLine(corners[5], corners[7]);
		lineRenderer->AddLine(corners[6], corners[7]);

		//Colliderの描画
		Collider::Draw(camera);
	}
}
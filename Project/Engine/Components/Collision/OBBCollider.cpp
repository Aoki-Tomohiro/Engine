#include "OBBCollider.h"
#include "Engine/3D/Primitive/LineRenderer.h"
#include "Engine/Math/MathFunction.h"

void OBBCollider::Initialize()
{

}

void OBBCollider::Update()
{
	if (transformComponent_ != nullptr)
	{
		//中心点
		center_ = {
			transformComponent_->worldTransform_.matWorld_.m[3][0] ,
			transformComponent_->worldTransform_.matWorld_.m[3][1] ,
			transformComponent_->worldTransform_.matWorld_.m[3][2]
		};

		//軸
		orientations_[0] = {
			transformComponent_->worldTransform_.matWorld_.m[0][0],
			transformComponent_->worldTransform_.matWorld_.m[0][1],
			transformComponent_->worldTransform_.matWorld_.m[0][2]
		};

		orientations_[1] = { 
			transformComponent_->worldTransform_.matWorld_.m[1][0],
			transformComponent_->worldTransform_.matWorld_.m[1][1],
			transformComponent_->worldTransform_.matWorld_.m[1][2] 
		};

		orientations_[2] = { 
			transformComponent_->worldTransform_.matWorld_.m[2][0],
			transformComponent_->worldTransform_.matWorld_.m[2][1],
			transformComponent_->worldTransform_.matWorld_.m[2][2] 
		};
	}
}

void OBBCollider::Draw(const Camera& camera)
{
	if (debugDrawEnabled_)
	{
		//頂点
		std::vector<Vector3> corners(8);

		//頂点を計算
		for (int i = 0; i < 8; ++i) {
			Vector3 vertex = center_;
			for (int j = 0; j < 3; ++j) {
				vertex.x += (i & (1 << j)) ? orientations_[j].x/* * size_.x*/ : -orientations_[j].x/* * size_.x*/;
				vertex.y += (i & (1 << j)) ? orientations_[j].y/* * size_.y*/ : -orientations_[j].y/* * size_.y*/;
				vertex.z += (i & (1 << j)) ? orientations_[j].z/* * size_.z*/ : -orientations_[j].z/* * size_.z*/;
			}
			corners[i] = vertex;
		}

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
	}
}
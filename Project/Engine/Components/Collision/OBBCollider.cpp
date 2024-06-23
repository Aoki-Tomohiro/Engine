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
		center_ = transformComponent_->worldTransform_.translation_;
		Matrix4x4 rotateMatrix = Mathf::MakeIdentity4x4();
		switch (transformComponent_->worldTransform_.rotationType_)
		{
		case RotationType::Euler:
			Matrix4x4 rotateXMatrix = Mathf::MakeRotateXMatrix(transformComponent_->worldTransform_.rotation_.x);
			Matrix4x4 rotateYMatrix = Mathf::MakeRotateYMatrix(transformComponent_->worldTransform_.rotation_.y);
			Matrix4x4 rotateZMatrix = Mathf::MakeRotateZMatrix(transformComponent_->worldTransform_.rotation_.z);
			rotateMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;
			break;
		case RotationType::Quaternion:
			rotateMatrix = Mathf::MakeRotateMatrix(transformComponent_->worldTransform_.quaternion_);
			break;
		}
		orientations_[0] = { rotateMatrix.m[0][0],rotateMatrix.m[0][1],rotateMatrix.m[0][2] };
		orientations_[1] = { rotateMatrix.m[1][0],rotateMatrix.m[1][1],rotateMatrix.m[1][2] };
		orientations_[2] = { rotateMatrix.m[2][0],rotateMatrix.m[2][1],rotateMatrix.m[2][2] };
		size_ = transformComponent_->worldTransform_.scale_;
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
				vertex.x += (i & (1 << j)) ? orientations_[j].x * size_.x : -orientations_[j].x * size_.x;
				vertex.y += (i & (1 << j)) ? orientations_[j].y * size_.y : -orientations_[j].y * size_.y;
				vertex.z += (i & (1 << j)) ? orientations_[j].z * size_.z : -orientations_[j].z * size_.z;
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
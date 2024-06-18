#include "OBBCollider.h"
#include "Engine/Math/MathFunction.h"

void OBBCollider::Initialize()
{

}

void OBBCollider::Update()
{
	if (transformComponent_ != nullptr)
	{
		center_ = transformComponent_->translation_;
		Matrix4x4 rotateMatrix = Mathf::MakeIdentity4x4();
		switch (transformComponent_->rotationType_)
		{
		case RotationType::Euler:
			Matrix4x4 rotateXMatrix = Mathf::MakeRotateXMatrix(transformComponent_->rotation_.x);
			Matrix4x4 rotateYMatrix = Mathf::MakeRotateYMatrix(transformComponent_->rotation_.y);
			Matrix4x4 rotateZMatrix = Mathf::MakeRotateZMatrix(transformComponent_->rotation_.z);
			rotateMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;
			break;
		case RotationType::Quaternion:
			rotateMatrix = Mathf::MakeRotateMatrix(transformComponent_->quaternion_);
			break;
		}
		orientations_[0] = { rotateMatrix.m[0][0],rotateMatrix.m[0][1],rotateMatrix.m[0][2] };
		orientations_[1] = { rotateMatrix.m[1][0],rotateMatrix.m[1][1],rotateMatrix.m[1][2] };
		orientations_[2] = { rotateMatrix.m[2][0],rotateMatrix.m[2][1],rotateMatrix.m[2][2] };
		size_ = transformComponent_->scale_;
	}
}
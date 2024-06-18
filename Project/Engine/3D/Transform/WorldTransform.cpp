#include "WorldTransform.h"
#include "Engine/Math/MathFunction.h"

void WorldTransform::Initialize()
{
	constBuff_ = std::make_unique<UploadBuffer>();
	constBuff_->Create(sizeof(ConstBuffDataWorldTransform));
	UpdateMatrix();
}

void WorldTransform::TransferMatrix()
{
	ConstBuffDataWorldTransform* worldTransformData = static_cast<ConstBuffDataWorldTransform*>(constBuff_->Map());
	worldTransformData->world = matWorld_;
	worldTransformData->worldInverseTranspse = Mathf::Transpose(Mathf::Inverse(matWorld_));
	constBuff_->Unmap();
}

void WorldTransform::UpdateMatrix()
{
	switch (rotationType_)
	{
	case RotationType::Euler:
		matWorld_ = Mathf::MakeAffineMatrix(scale_, rotation_, translation_);
		break;
	case RotationType::Quaternion:
		matWorld_ = Mathf::MakeAffineMatrix(scale_, quaternion_, translation_);
		break;
	}

	if (parent_)
	{
		matWorld_ = matWorld_ * parent_->matWorld_;
	}

	TransferMatrix();
}
/**
 * @file WorldTransform.cpp
 * @brief ワールドトランスフォームを管理するファイル
 * @author 青木智滉
 * @date
 */

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
	//回転のタイプに応じて行列の計算を変える
	switch (rotationType_)
	{
	case RotationType::Euler:
		//原点オフセットを回転させる
		cachedOriginOffset_ = Mathf::TransformNormal(originOffset_, Mathf::MakeRotateMatrix(rotation_));
		//アフィン行列の計算
		matWorld_ = Mathf::MakeAffineMatrix(scale_, rotation_, translation_ + cachedOriginOffset_);
		break;
	case RotationType::Quaternion:
		//原点オフセットを回転させる
		cachedOriginOffset_ = Mathf::RotateVector(originOffset_, quaternion_);
		//アフィン行列の計算
		matWorld_ = Mathf::MakeAffineMatrix(scale_, quaternion_, translation_ + cachedOriginOffset_);
		break;
	}

	//親がいる場合は親の行列をかける
	if (parent_)
	{
		matWorld_ = matWorld_ * parent_->matWorld_;
	}

	//行列の転送
	TransferMatrix();
}

void WorldTransform::SetParent(const WorldTransform* parent)
{
	//親を設定
	parent_ = parent;
	//親基準のローカル座標を計算
	if (parent_)
	{
		translation_ -= parent->translation_;
	}
}

void WorldTransform::UnsetParent()
{
	if (parent_)
	{
		//新しい座標を設定
		translation_ = { Vector3{matWorld_.m[3][0],matWorld_.m[3][1],matWorld_.m[3][2]} };
	}
	//親子付けを外す
	parent_ = nullptr;
}

const Vector3 WorldTransform::GetWorldPosition() const
{
	Vector3 worldPosition{};
	worldPosition.x = matWorld_.m[3][0];
	worldPosition.y = matWorld_.m[3][1];
	worldPosition.z = matWorld_.m[3][2];
	return worldPosition - cachedOriginOffset_;
}
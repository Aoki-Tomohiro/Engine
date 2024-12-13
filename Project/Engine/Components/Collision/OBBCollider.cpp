/**
 * @file OBBCollider.cpp
 * @brief OBBコライダーを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "OBBCollider.h"
#include "Engine/3D/Primitive/LineRenderer.h"
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Math/MathFunction.h"

void OBBCollider::Update()
{
	if (!isWorldCenterSet_)
	{
		//トランスフォームコンポーネントからワールド座標を計算
		TransformComponent* transformComponent = owner_->GetComponent<TransformComponent>();
		worldCenter_ = transformComponent->GetWorldPosition() + center_;
	}

	//OBBの軸を正規化
	orientations_[0] = Mathf::Normalize(orientations_[0]);
	orientations_[1] = Mathf::Normalize(orientations_[1]);
	orientations_[2] = Mathf::Normalize(orientations_[2]);
}

void OBBCollider::Draw(const Camera& camera)
{
	if (debugDrawEnabled_)
	{
		//頂点
		std::vector<Vector3> corners(8);

		//行列の計算
		Matrix4x4 rotateMatrix = Mathf::MakeIdentity4x4();
		rotateMatrix.m[0][0] = orientations_[0].x;
		rotateMatrix.m[0][1] = orientations_[0].y;
		rotateMatrix.m[0][2] = orientations_[0].z;
		rotateMatrix.m[1][0] = orientations_[1].x;
		rotateMatrix.m[1][1] = orientations_[1].y;
		rotateMatrix.m[1][2] = orientations_[1].z;
		rotateMatrix.m[2][0] = orientations_[2].x;
		rotateMatrix.m[2][1] = orientations_[2].y;
		rotateMatrix.m[2][2] = orientations_[2].z;
		Matrix4x4 worldMatrix = Mathf::MakeScaleMatrix(size_) * rotateMatrix * Mathf::MakeTranslateMatrix(worldCenter_);

		//軸
		Vector3 orientations[3] = { {1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f} };
		orientations[0] = {
			worldMatrix.m[0][0],
			worldMatrix.m[0][1],
			worldMatrix.m[0][2],
		};
		orientations[1] = {
			worldMatrix.m[1][0],
			worldMatrix.m[1][1],
			worldMatrix.m[1][2],
		};
		orientations[2] = {
			worldMatrix.m[2][0],
			worldMatrix.m[2][1],
			worldMatrix.m[2][2],
		};

		//頂点を計算
		for (int i = 0; i < 8; ++i) {
			Vector3 vertex = worldCenter_;
			for (int j = 0; j < 3; ++j) {
				vertex.x += (i & (1 << j)) ? orientations[j].x : -orientations[j].x;
				vertex.y += (i & (1 << j)) ? orientations[j].y : -orientations[j].y;
				vertex.z += (i & (1 << j)) ? orientations[j].z : -orientations[j].z;
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

		//Colliderの描画
		Collider::Draw(camera);
	}
}
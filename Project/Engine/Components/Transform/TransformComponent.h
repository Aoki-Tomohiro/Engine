/**
 * @file TransformComponent.h
 * @brief トランスフォームのコンポーネント
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Components/Base/Component.h"
#include "Engine/3D/Transform/WorldTransform.h"

class TransformComponent : public Component
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns>ワールド座標</returns>
	const Vector3 GetWorldPosition() const;

	//ワールドトランスフォーム
	WorldTransform worldTransform_{};
};


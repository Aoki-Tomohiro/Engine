/**
 * @file RenderComponent.h
 * @brief 描画をするコンポーネントの基底クラスを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Component.h"
#include "Engine/3D/Camera/Camera.h"

class RenderComponent : public Component
{
public:
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	virtual void Draw(const Camera& camera) = 0;
};


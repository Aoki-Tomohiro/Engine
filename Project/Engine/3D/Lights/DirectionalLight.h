/**
 * @file DirectionalLight.h
 * @brief 平行光源を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Vector4.h"

class DirectionalLight
{
public:
	//色を取得・設定
	const Vector4& GetColor() const { return color_; };
	void SetColor(const Vector4& color) { color_ = color; };

	//方向を取得・設定
	const Vector3& GetDirection() const { return direction_; };
	void SetDirection(const Vector3& direction) { direction_ = direction; };

	//強さを取得・設定
	const float GetIntensity() const { return intensity_; };
	void SetIntensity(const float intensity) { intensity_ = intensity; };

	//有効にするかを取得・設定
	const bool GetIsEnable() const { return isEnable_; };
	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

private:
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	Vector3 direction_ = { 0.0f,-1.0f,0.0f };

	float intensity_ = 1.0f;

	bool isEnable_ = true;
};


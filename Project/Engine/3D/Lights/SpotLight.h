/**
 * @file SpotLight.h
 * @brief スポットライトを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Vector4.h"

class SpotLight
{
public:
	//色を取得・設定
	const Vector4& GetColor() const { return color_; };
	void SetColor(const Vector4& color) { color_ = color; };

	//座標を取得・設定
	const Vector3& GetPosition() const { return position_; };
	void SetPosition(const Vector3& position) { position_ = position; };

	//強さを取得・設定
	const float GetIntensity() const { return intensity_; };
	void SetIntensity(const float intensity) { intensity_ = intensity; };

	//方向を取得・設定
	const Vector3& GetDirection() const { return direction_; };
	void SetDirection(const Vector3& direction) { direction_ = direction; };

	//距離を取得・設定
	const float GetDistance() const { return distance_; };
	void SetDistance(const float distance) { distance_ = distance; };

	//減衰率を取得・設定
	const float GetDecay() const { return decay_; };
	void SetDecay(const float decay) { decay_ = decay; };

	//コサインフォールオフの開始値を取得・設定
	const float GetCosFalloffStart() const { return cosFalloffStart_; };
	void SetCosFalloffStart(const float cosFalloffStart) { cosFalloffStart_ = cosFalloffStart; };

	//角度を取得・設定
	const float GetCosAngle() const { return cosAngle_; };
	void SetCosAngle(const float cosAngle) { cosAngle_ = cosAngle; };

	//有効にするかを取得・設定
	const bool GetIsEnable() const { return isEnable_; };
	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

private:
	Vector4 color_{ 1.0f,1.0f,1.0f,1.0f };

	Vector3 position_ = { 0.0f,2.0f,0.0f };

	float intensity_ = 1.0f;

	Vector3 direction_ = { 0.0f,-1.0f,0.0f };

	float distance_ = 10.0f;

	float decay_ = 1.0f;

	float cosFalloffStart_ = 1.0f;

	float cosAngle_ = 0.5f;

	bool isEnable_ = false;
};


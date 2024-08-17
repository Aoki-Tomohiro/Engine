#pragma once
#include "Engine/Math/MathFunction.h"
#include <vector>

struct CameraKeyFrame
{
	float time = 0.0f;                             //時間
	Vector3 position = { 0.0f,2.0f,-10.0f };       //座標
	Quaternion rotation = { 0.0f,0.0f,0.0f,1.0f }; //回転
	float fov = 45.0f;                             //視野角
};

class CameraPath
{
public:
	void AddKeyFrame(const CameraKeyFrame& cameraKeyFrame);

	void RemoveKeyFrame(const size_t index);

	const CameraKeyFrame GetInterpolatedKeyFrame(const float time) const;

	CameraKeyFrame& GetCameraKeyFrame(const size_t index) { return keyFrames_[index]; };

	const float GetDuration() const;

	const size_t GetKeyFrameCount() const { return keyFrames_.size(); };

private:
	std::vector<CameraKeyFrame> keyFrames_{};
};


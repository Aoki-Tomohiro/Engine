/**
 * @file CameraPath.cpp
 * @brief カメラパスを制御するファイル
 * @author 青木智滉
 * @date
 */

#include "CameraPath.h"

const CameraPath::CameraKeyFrame CameraPath::GetInterpolatedKeyFrame(const float time) const
{
	//キーフレームがない場合はデフォルトのキーフレームを返す
	if (keyFrames_.empty())
	{
		return CameraKeyFrame();
	}

	//キーフレームが1つしかない場合そのキーフレームを返す
	if (keyFrames_.size() == 1)
	{
		return keyFrames_.front();
	}

	//キーフレームの補間
	for (size_t i = 0; i < keyFrames_.size() - 1; ++i)
	{
		if (time >= keyFrames_[i].time && time <= keyFrames_[i + 1].time)
		{
			CameraKeyFrame result;
			float t = (time - keyFrames_[i].time) / (keyFrames_[i + 1].time - keyFrames_[i].time);
			t = GetEasingParameter(keyFrames_[i].easingType, t);
			result.position = Mathf::Lerp(keyFrames_[i].position, keyFrames_[i + 1].position, t);
			result.rotation = Mathf::Slerp(keyFrames_[i].rotation, keyFrames_[i + 1].rotation, t);
			result.fov = Mathf::Lerp(keyFrames_[i].fov, keyFrames_[i + 1].fov, t);
			return result;
		}
	}

	//最後のキーフレームを返す
	return keyFrames_.back();
}

const float CameraPath::GetDuration() const
{
	return keyFrames_.empty() ? 0.0f : keyFrames_.back().time;
}

const float CameraPath::GetEasingParameter(const EasingType easingType, const float animationTime) const
{
	//イージングの種類に応じた処理
	switch (easingType)
	{
	case EasingType::kEaseIn:
		return Mathf::EaseInSine(animationTime);
	case EasingType::kEaseOut:
		return Mathf::EaseOutSine(animationTime);
	case EasingType::kEaseInOut:
		return Mathf::EaseInOutSine(animationTime);
	default:
		return animationTime;
	}
}

void CameraPath::AddKeyFrame(const CameraKeyFrame& cameraKeyFrame)
{
	keyFrames_.push_back(cameraKeyFrame);
}

void CameraPath::RemoveKeyFrame(const size_t index)
{
	if (index < keyFrames_.size())
	{
		keyFrames_.erase(keyFrames_.begin() + index);
	}
}

void CameraPath::ClearKeyFrames()
{
	keyFrames_.clear();
}
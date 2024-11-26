#include "CameraPath.h"

const CameraKeyFrame CameraPath::GetInterpolatedKeyFrame(const float time) const
{
	//キーフレームがない場合はデフォルトのキーフレームを返す
	if (keyFrames_.empty())
	{
		return CameraKeyFrame();
	}

	//パスの範囲内の時間に制限
	float duration = GetDuration();
	float currentTime = std::fmod(std::max(time, 0.0f), duration);

	//キーフレームが1つしかない場合そのキーフレームを返す
	if (keyFrames_.size() == 1)
	{
		return keyFrames_.front();
	}

	//キーフレームの補間
	for (size_t i = 0; i < keyFrames_.size() - 1; ++i)
	{
		if (currentTime >= keyFrames_[i].time && currentTime <= keyFrames_[i + 1].time)
		{
			CameraKeyFrame result;
			float t = (currentTime - keyFrames_[i].time) / (keyFrames_[i + 1].time - keyFrames_[i].time);
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
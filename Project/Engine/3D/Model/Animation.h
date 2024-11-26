#pragma once
#include "Model.h"
#include <map>
#include <optional>
#include <string>
#include <vector>

class Animation
{
public:
	//Keyframe構造体
	template <typename tValue>
	struct Keyframe 
	{
		float time;
		tValue value;
	};
	using KeyframeVector3 = Keyframe<Vector3>;
	using KeyframeQuaternion = Keyframe<Quaternion>;

	//AnimationCurve構造体
	template <typename tValue>
	struct AnimationCurve
	{
		std::vector<Keyframe<tValue>> keyframes;
	};

	//NodeAnimation構造体
	struct NodeAnimation 
	{
		AnimationCurve<Vector3> translate;
		AnimationCurve<Quaternion> rotate;
		AnimationCurve<Vector3> scale;
	};

	//Animation構造体
	struct AnimationData 
	{
		//アニメーションの名前
		std::string name;
		//アニメーション全体の尺(単位は秒)
		float duration;
		//NodeAnimationの集合。Node名でひけるようにしておく
		std::map<std::string, NodeAnimation> nodeAnimations;
	};

	void Initialize(const std::vector<AnimationData>& animationData);

	void UpdateAnimationTime();

	void ApplyAnimation(Model* model, WorldTransform& worldTransform, const Vector3& inPlaceAxis = { 1.0f, 1.0f, 1.0f });

	void ApplyBlendAnimation(Model* model, WorldTransform& worldTransform, Animation* animation, const float blendFactor, const Vector3& inPlaceAxis = { 1.0f, 1.0f, 1.0f });

	void PlayAnimation(const std::string& animationName, const float speed, const bool loop);

	void PlayAnimation(const float speed, const bool loop);

	void StopAnimation();

	void PauseAnimation();

	void ResumeAnimation();

	const float GetAnimationTime() const { return animationTime_; };

	void SetAnimationTime(const float animationTime) { animationTime_ = animationTime; };

	const float GetAnimationSpeed() const { return animationSpeed_; };

	void SetAnimationSpeed(const float animationSpeed) { animationSpeed_ = animationSpeed; };

	const bool GetLoop() const { return loop_; };

	void SetLoop(const bool loop) { loop_ = loop; };

	const bool GetIsAnimationFinished() const { return isAnimationFinished_; };

	const float GetDuration() const;

	const AnimationData* GetAnimationData() const;

private:
	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

	void ApplyNodeAnimation(Model* model, WorldTransform& worldTransform, const AnimationData& animationData);

	void ApplySkeletonAnimation(Model* model, const AnimationData& animationData, const Vector3& inPlaceAxis);

	void ApplyBlendedNodeAnimation(Model* model, WorldTransform& worldTransform, const AnimationData& currentAnimationData, const Animation* blendAnimation, const float blendFactor);

	void ApplyBlendedSkeletonAnimation(Model* model, const AnimationData& currentAnimationData, const Animation* blendAnimation, const float blendFactor, const Vector3& inPlaceAxis);

private:
	//アニメーションデータ
	std::vector<AnimationData> animationDatas_{};

	//アニメーションの名前
	std::string animationName_{};

	//アニメーションを停止させるかどうか
	bool stop_ = false;

	//アニメーションを一時停止させるかどうか
	bool pause_ = false;

	//ループさせるかどうか
	bool loop_ = true;
	
	//アニメーションが終了したかどうか
	bool isAnimationFinished_ = false;

	//アニメーションの時間
	float animationTime_ = 0.0f;

	//アニメーションの速度
	float animationSpeed_ = 1.0f;
};


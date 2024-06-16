#pragma once
#include "Engine/3D/Transform/WorldTransform.h"
#include <map>
#include <optional>
#include <string>
#include <vector>

class Animation
{
public:
	struct Node
	{
		Vector3 scale;
		Quaternion rotate;
		Vector3 translate;
		Matrix4x4 localMatrix{};
		std::string name;
		std::vector<Node> children;
	};

	//Keyframe構造体
	template <typename tValue>
	struct Keyframe {
		float time;
		tValue value;
	};
	using KeyframeVector3 = Keyframe<Vector3>;
	using KeyframeQuaternion = Keyframe<Quaternion>;

	//AnimationCurve構造体
	template <typename tValue>
	struct AnimationCurve {
		std::vector<Keyframe<tValue>> keyframes;
	};

	//NodeAnimation構造体
	struct NodeAnimation {
		AnimationCurve<Vector3> translate;
		AnimationCurve<Quaternion> rotate;
		AnimationCurve<Vector3> scale;
	};

	//Animation構造体
	struct AnimationData {
		float duration;//アニメーション全体の尺(単位は秒)
		//NodeAnimationの集合。Node名でひけるようにしておく
		std::map<std::string, NodeAnimation> nodeAnimations;
		//Animationの名前
		std::string name;
	};

	//Joint構造体
	struct Joint
	{
		Vector3 scale;//scale
		Quaternion rotate;//rotate
		Vector3 translate;//translate
		Matrix4x4 localMatrix;//localMatrix
		Matrix4x4 skeletonSpaceMatrix;//SkeletonSpaceでの変換行列
		std::string name;//名前
		std::vector<int32_t> children;//子JointのIndexのリスト。いなければ空
		int32_t index;//自身のIndex
		std::optional<int32_t> parent;//親JointのIndex。いなければnull
	};

	//スケルトン構造体
	struct Skeleton
	{
		int32_t root;//RootJointのIndex
		std::map<std::string, int32_t> jointMap;//Joint名とIndexとの辞書
		std::vector<Joint> joints;//所属しているジョイント
	};

	void Initialize(const std::vector<AnimationData>& animationData, const Node& rootNode);

	void Update();

	void ApplyAnimation(WorldTransform& worldTransform, const std::string& rootNodeName, const std::string& animationName);

	const Skeleton& GetSkeleton() const { return skeletonData_; };

private:
	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

	Skeleton CreateSkeleton(const Node& rootNode);

	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

private:
	std::vector<AnimationData> animationData_{};

	Skeleton skeletonData_{};

	float animationTime_ = 0.0f;
};


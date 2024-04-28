#pragma once
#include <map>
#include <string>
#include <vector>
#include <optional>
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Quaternion.h"
#include "Engine/Math/Matrix4x4.h"

class Animation
{
public:
	//ノード構造体
	struct Node {
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
		//アニメーションがあるか
		bool containsAnimation;
	};

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

	struct Skeleton
	{
		int32_t root;//RootJointのIndex
		std::map<std::string, int32_t> jointMap;//Joint名とIndexとの辞書
		std::vector<Joint> joints;//所属しているジョイント
	};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="animationData"></param>
	void Initialize(const AnimationData& animationData, const Node& rootNode);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const std::string& name);

	/// <summary>
	/// RigidAnimationの再生
	/// </summary>
	/// <param name="name"></param>
	void PlayRigidAnimation();

	/// <summary>
	/// RigidAnimationの一時停止
	/// </summary>
	void PauseRigidAnimation();

	/// <summary>
	/// RigidAnimationの停止
	/// </summary>
	void StopRigidAnimation();

	/// <summary>
	/// 再生速度(フレーム)を設定
	/// </summary>
	/// <param name="speed"></param>
	void SetSpeed(const float speed) { speed_ = speed; }

	/// <summary>
	/// ループ再生のフラグを設定
	/// </summary>
	/// <param name="isLoop"></param>
	void SetLoop(const bool isLoop) { isLoop_ = isLoop; };

	/// <summary>
	/// アニメーションの再生中か
	/// </summary>
	/// <returns></returns>
	const bool IsPlaying() const { return isPlay_; };

	/// <summary>
	/// アニメーションの一時停止中か
	/// </summary>
	/// <returns></returns>
	const bool IsPaused() const { return isPause_; };

	/// <summary>
	/// ループ再生中か
	/// </summary>
	/// <returns></returns>
	const bool IsLooping() const { return isLoop_; };

	/// <summary>
	/// ローカル座標を取得
	/// </summary>
	/// <returns></returns>
	const Matrix4x4& GetLocalMatrix() const { return localMatrix_; };

private:
	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
	
	Skeleton CreateSkeleton(const Node& rootNode);

	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

private:
	AnimationData animationData_{};

	Skeleton skeletonData_{};

	Matrix4x4 localMatrix_{};

	float animationTime_ = 0.0f;

	//再生速度
	float speed_ = 60.0f;

	//ループ再生中か
	bool isLoop_ = false;

	//再生中か
	bool isPlay_ = false;

	//一時停止中か
	bool isPause_ = false;
};


#pragma once
#include <map>
#include <string>
#include <vector>
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Quaternion.h"
#include "Engine/Math/Matrix4x4.h"

class Animation
{
public:
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

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="animationData"></param>
	void Initialize(const AnimationData& animationData);

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

private:
	AnimationData animationData_{};

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


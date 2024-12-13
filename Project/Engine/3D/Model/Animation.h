/**
 * @file Animation.h
 * @brief アニメーションを管理するファイル
 * @author 青木智滉
 * @date
 */

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

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="animationData">アニメーションデータ</param>
	void Initialize(const std::vector<AnimationData>& animationData);

	/// <summary>
	/// アニメーションの時間を更新
	/// </summary>
	void UpdateAnimationTime();

	/// <summary>
	/// アニメーションを適用
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="inPlaceAxis">動かす軸</param>
	void ApplyAnimation(Model* model, WorldTransform& worldTransform, const Vector3& inPlaceAxis = { 1.0f, 1.0f, 1.0f });

	/// <summary>
	/// アニメーションブレンドをしたアニメーションを適用
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="animation">アニメーション</param>
	/// <param name="blendFactor">ブレンド係数</param>
	/// <param name="inPlaceAxis">動かす軸</param>
	void ApplyBlendAnimation(Model* model, WorldTransform& worldTransform, Animation* animation, const float blendFactor, const Vector3& inPlaceAxis = { 1.0f, 1.0f, 1.0f });

	/// <summary>
	/// アニメーションを再生
	/// </summary>
	/// <param name="animationName">アニメーションの名前</param>
	/// <param name="speed">アニメーションの速度</param>
	/// <param name="loop">ループフラグ</param>
	void PlayAnimation(const std::string& animationName, const float speed, const bool loop);

	/// <summary>
	/// アニメーションを再生
	/// </summary>
	/// <param name="speed">アニメーション速度</param>
	/// <param name="loop">ループフラグ</param>
	void PlayAnimation(const float speed, const bool loop);

	/// <summary>
	/// アニメーションを停止
	/// </summary>
	void StopAnimation();

	/// <summary>
	/// アニメーションを一時停止
	/// </summary>
	void PauseAnimation();

	/// <summary>
	/// アニメーションを再開
	/// </summary>
	void ResumeAnimation();

	//アニメーションの時間を取得・設定
	const float GetAnimationTime() const { return animationTime_; };
	void SetAnimationTime(const float animationTime) { animationTime_ = animationTime; };

	//アニメーション速度を取得・設定
	const float GetAnimationSpeed() const { return animationSpeed_; };
	void SetAnimationSpeed(const float animationSpeed) { animationSpeed_ = animationSpeed; };

	//ループフラグを取得・設定
	const bool GetLoop() const { return loop_; };
	void SetLoop(const bool loop) { loop_ = loop; };

	//アニメーションが終了しているかを取得
	const bool GetIsAnimationFinished() const { return isAnimationFinished_; };

	//アニメーションの持続時間を取得
	const float GetDuration() const;

	//アニメーションデータを取得
	const AnimationData* GetAnimationData() const;

private:
	/// <summary>
	/// キーフレームを基に値を計算（Vector3）
	/// </summary>
	/// <param name="keyframes">キーフレーム</param>
	/// <param name="time">アニメーションの時間</param>
	/// <returns>計算された値</returns>
	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

	/// <summary>
	/// キーフレームを基に値を計算（Quaternion）
	/// </summary>
	/// <param name="keyframes">キーフレーム</param>
	/// <param name="time">アニメーションの時間</param>
	/// <returns>計算された値</returns>
	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

	/// <summary>
	/// ノードアニメーションを適用
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="animationData">アニメーションデータ</param>
	void ApplyNodeAnimation(Model* model, WorldTransform& worldTransform, const AnimationData& animationData);

	/// <summary>
	/// スケルトンアニメーションを適用
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="animationData">アニメーションデータ</param>
	/// <param name="inPlaceAxis">動かす軸</param>
	void ApplySkeletonAnimation(Model* model, const AnimationData& animationData, const Vector3& inPlaceAxis);

	/// <summary>
	/// ブレンドしたノードアニメーションを適用
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="currentAnimationData">現在のアニメーションデータ</param>
	/// <param name="blendAnimation">ブレンドするアニメーションデータ</param>
	/// <param name="blendFactor">ブレンド係数</param>
	void ApplyBlendedNodeAnimation(Model* model, WorldTransform& worldTransform, const AnimationData& currentAnimationData, const Animation* blendAnimation, const float blendFactor);

	/// <summary>
	/// ブレンドしたスケルトンアニメーションを適用
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="currentAnimationData">現在のアニメーションデータ</param>
	/// <param name="blendAnimation">ブレンドするアニメーションデータ</param>
	/// <param name="blendFactor">ブレンド係数</param>
	/// <param name="inPlaceAxis">動かす軸</param>
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


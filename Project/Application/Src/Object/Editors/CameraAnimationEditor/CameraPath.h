/**
 * @file CameraPath.h
 * @brief カメラパスを制御するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Math/MathFunction.h"
#include <vector>

class CameraPath
{
public:
	//イージングの種類
	enum class EasingType
	{
		kLinear,    //リニア
		kEaseIn,    //イーズイン
		kEaseOut,   //イーズアウト
		kEaseInOut, //イーズインアウト
	};

	//キーフレーム
	struct CameraKeyFrame
	{
		float time = 0.0f;                                //時間
		Vector3 position = { 0.0f, 1.0f, -10.0f };        //座標
		Quaternion rotation = { 0.0f, 0.0f, 0.0f, 1.0f }; //回転
		float fov = 45.0f;                                //視野角
		EasingType easingType = EasingType::kLinear;      //イージングの種類
	};

	/// <summary>
	/// キーフレームを追加
	/// </summary>
	/// <param name="cameraKeyFrame">キーフレーム</param>
	void AddKeyFrame(const CameraKeyFrame& cameraKeyFrame);

	/// <summary>
	/// キーフレームを削除
	/// </summary>
	/// <param name="index">キーフレームのインデックス</param>
	void RemoveKeyFrame(const size_t index);

	/// <summary>
	/// キーフレームをクリア
	/// </summary>
	void ClearKeyFrames();

	/// <summary>
	/// 補間したキーフレームを取得
	/// </summary>
	/// <param name="time">アニメーションの時間</param>
	/// <returns>補間したキーフレーム</returns>
	const CameraKeyFrame GetInterpolatedKeyFrame(const float time) const;

	/// <summary>
	/// キーフレームを取得
	/// </summary>
	/// <param name="index">キーフレームのインデックス</param>
	/// <returns>キーフレーム</returns>
	CameraKeyFrame& GetCameraKeyFrame(const size_t index) { return keyFrames_[index]; };

	/// <summary>
	/// 持続時間を取得
	/// </summary>
	/// <returns>持続時間</returns>
	const float GetDuration() const;

	/// <summary>
	/// キーフレームの数を取得
	/// </summary>
	/// <returns>キーフレームの数</returns>
	const size_t GetKeyFrameCount() const { return keyFrames_.size(); };

	//補間を戻す際のイージングタイプを取得・設定
	const EasingType GetResetEasingType() const { return resetEasingType_; };
	void SetResetEasingType(const EasingType easingType) { resetEasingType_ = easingType; };

	//補間を戻す時間を取得・設定
	const float GetResetDuration() const { return resetDuration_; };
	void SetResetDuration(const float resetDuration) { resetDuration_ = resetDuration; };

private:
	/// <summary>
	/// イージング係数を取得
	/// </summary>
	/// <param name="easingType">イージングタイプ</param>
	/// <param name="animationTime">アニメーションの時間</param>
	/// <returns>イージング係数</returns>
	const float GetEasingParameter(const EasingType easingType, const float animationTime) const;

private:
	//キーフレーム
	std::vector<CameraKeyFrame> keyFrames_{};

	//補間速度リセット時のイージングタイプ
	EasingType resetEasingType_ = EasingType::kLinear;

	//補間速度リセットが完了するまでの時間
	float resetDuration_ = 1.0f;
};


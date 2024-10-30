#pragma once
#include "Engine/Math/MathFunction.h"
#include <vector>

//キーフレーム
struct CameraKeyFrame
{
	float time = 0.0f;                             //時間
	Vector3 position = { 0.0f,2.0f,-10.0f };       //座標
	Quaternion rotation = { 0.0f,0.0f,0.0f,1.0f }; //回転
	float fov = 45.0f;                             //視野角
};

/// <summary>
/// カメラパスを制御するクラス
/// </summary>
class CameraPath
{
public:
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

private:
	std::vector<CameraKeyFrame> keyFrames_{};
};


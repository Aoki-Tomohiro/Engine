/**
 * @file GameTimer.h
 * @brief ゲーム時間を管理するクラス
 * @author 青木智滉
 * @date
 */

#pragma once

class GameTimer
{
public:
	/// <summary>
	/// 更新
	/// </summary>
	static void Update();

	//経過時間を取得
	static const float GetElapsedTime() { return currentTime_; };

	//デルタタイムを取得
	static const float GetDeltaTime() { return deltaTime_; };

	//タイムスケールを取得、設定
	static const float GetTimeScale() { return timeScale_; };
	static void SetTimeScale(const float timeScale) { timeScale_ = timeScale; };

private:
	static float currentTime_;

	static float deltaTime_;

	static float timeScale_;
};


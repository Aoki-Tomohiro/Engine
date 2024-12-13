/**
 * @file FrameRateController.h
 * @brief FPSを制御するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include <chrono>
#include <thread>

class FrameRateController
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

private:
	std::chrono::steady_clock::time_point reference_{};
};


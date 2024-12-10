/**
 * @file RandomGenerator.h
 * @brief ランダムな値を生成するクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include <random>

class RandomGenerator
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	static void Initialize();

	/// <summary>
	/// ランダムなint型の値を取得
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>ランダムなint型の値</returns>
	static int GetRandomInt(int min, int max);

	/// <summary>
	/// ランダムなfloat型の値を取得
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>ランダムなfloat型の値</returns>
	static float GetRandomFloat(float min, float max);

private:
	static std::mt19937 randomEngine_;
};


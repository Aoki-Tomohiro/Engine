/**
 * @file GameTimer.cpp
 * @brief ゲーム時間を管理するクラス
 * @author 青木智滉
 * @date
 */

#include "GameTimer.h"
#include <chrono>

float GameTimer::currentTime_ = 0.0f;
float GameTimer::deltaTime_ = 0.0f;
float GameTimer::timeScale_ = 1.0f;

void GameTimer::Update()
{
    //現在の時間を取得
    static auto start = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    currentTime_ = std::chrono::duration<float>(now - start).count();

    //DeltaTimeを計算
    deltaTime_ = 1.0f / 60.0f;

    //DeltaTimeにスケールを適用
    deltaTime_ *= timeScale_;
}
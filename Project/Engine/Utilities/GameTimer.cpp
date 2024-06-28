#include "GameTimer.h"
#include <chrono>

float GameTimer::currentTime_ = 0.0f;
float GameTimer::deltaTime_ = 0.0f;
float GameTimer::lastFrameTime_ = 0.0f;

void GameTimer::Update()
{
    //現在の時間を取得
    static auto start = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    currentTime_ = std::chrono::duration<float>(now - start).count();

    //DeltaTimeを計算
    deltaTime_ = currentTime_ - lastFrameTime_;

    //前のフレームの時間を更新
    lastFrameTime_ = currentTime_;
}
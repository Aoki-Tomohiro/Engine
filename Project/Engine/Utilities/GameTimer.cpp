#include "GameTimer.h"
#include <Windows.h>

float GameTimer::currentTime_ = 0.0f;
float GameTimer::deltaTime_ = 0.0f;
float GameTimer::lastFrameTime_ = 0.0f;

void GameTimer::Update()
{
    float currentTime = GetTime();
    deltaTime_ = currentTime - lastFrameTime_;
    lastFrameTime_ = currentTime;
    currentTime_ += deltaTime_;
}

float GameTimer::GetTime()
{
    LARGE_INTEGER frequency;
    LARGE_INTEGER currentTime;

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&currentTime);

    return static_cast<float>(currentTime.QuadPart) / frequency.QuadPart;
}
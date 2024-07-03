#pragma once
class GameTimer
{
public:
	static void Update();

	static const float GetElapsedTime() { return currentTime_; };

	static const float GetDeltaTime() { return deltaTime_; };

	static const float GetTimeScale() { return timeScale_; };

	static void SetTimeScale(const float timeScale) { timeScale_ = timeScale; };

private:
	static float currentTime_;

	static float deltaTime_;

	static float lastFrameTime_;

	static float timeScale_;
};


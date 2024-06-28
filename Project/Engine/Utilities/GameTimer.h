#pragma once
class GameTimer
{
public:
	static void Update();

	static const float GetElapsedTime() { return currentTime_; };

	static const float GetDeltaTime() { return deltaTime_; };

private:
	static float GetTime();

private:
	static float currentTime_;

	static float deltaTime_;

	static float lastFrameTime_;
};


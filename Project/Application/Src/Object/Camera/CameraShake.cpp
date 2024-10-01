#include "CameraShake.h"

void CameraShake::Start(const Vector3& intensity, const float duration)
{
	isActive_ = true; // カメラシェイクを有効にする
	duration_ = duration; // カメラシェイクの持続時間を設定
	elapsedTime_ = duration; // 初期経過時間を持続時間で設定
	intensity_ = intensity; // カメラシェイクの強度を設定
}

void CameraShake::Update()
{
	if (isActive_)
	{
		//経過時間を減らす
		const float kDeltaTime = 1.0f / 60.0f;
		elapsedTime_ -= kDeltaTime;

		//経過時間が0以下になった場合はシェイクを無効にし、オフセットをリセット
		if (elapsedTime_ <= 0.0f)
		{
			isActive_ = false;
			shakeOffset_ = { 0.0f, 0.0f, 0.0f };
			return;
		}

		//現在の進行状況を計算
		float currentTime = (duration_ - elapsedTime_) / duration_;

		//イージング関数を適用して、シェイクの強さを滑らかに減衰させる
		float easedProgress = Mathf::EaseOutExpo(currentTime);

		//現在のカメラシェイクの強度を設定
		Vector3 currentIntensity = intensity_ * easedProgress;

		//シェイクオフセットを計算
		shakeOffset_ = {
			RandomGenerator::GetRandomFloat(-currentIntensity.x,currentIntensity.x),
			RandomGenerator::GetRandomFloat(-currentIntensity.y,currentIntensity.y),
			RandomGenerator::GetRandomFloat(-currentIntensity.z,currentIntensity.z),
		};
	}
}
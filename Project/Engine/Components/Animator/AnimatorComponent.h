#pragma once
#include "Engine/3D/Model/Animation.h"
#include "Engine/Components/Base/Component.h"

class AnimatorComponent : public Component
{
public:
	void Initialize() override;

	void Update() override;

	void AddAnimation(const std::string& animationName, Animation* animation);

	void PlayAnimation(const std::string& animationName, const float speed, const bool loop);

	void StopAnimation();

	const bool GetIsAnimationFinished(const std::string& animationName) const;

	const bool GetIsAnimationFinished();

	const float GetAnimationDuration(const std::string& animationName) const;

	const float GetAnimationDuration();

	const float GetAnimationTime(const std::string& animationName) const;

	void SetAnimationTime(const std::string& animationName, const float animationTime);

	const float GetAnimationTime() const;

	void SetAnimationTime(const float animationTime);

	const bool GetIsDlending() const { return isBlending_; };

	void SetIsBlending(const bool isBlending) { isBlending_ = isBlending; };

	const float GetBlendFactor() const { return blendFactor_; };

	void SetBlendFactor(const float blendFactor) { blendFactor_ = blendFactor; };

	const float GetBlendDuration() const { return blendDuration_; };

	void SetBlendDuration(const float blendDuration) { blendDuration_ = blendDuration; };

	Animation* GetAnimation(const std::string& animationName) const;

private:
	//アニメーションのマップ
	std::map<std::string, std::unique_ptr<Animation>> animations_{};

	//現在のアニメーションの名前
	std::string currentAnimation_{};

	//次のアニメーションの名前
	std::string nextAnimation_{};

	//アニメーションをブレンドするかどうか
	bool isBlending_ = true;

	//アニメーションのブレンド係数
	float blendFactor_ = 0.0f;

	//アニメーションブレンドの持続時間
	float blendDuration_ = 0.2f;
};


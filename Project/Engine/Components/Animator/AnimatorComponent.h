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

private:
	std::map<std::string, std::unique_ptr<Animation>> animations_{};

	std::string currentAnimation_{};

	std::string nextAnimation_{};

	bool isBlending_ = false;

	float blendFactor_ = 0.0f;

	float blendDuration_ = 1.0f;
};


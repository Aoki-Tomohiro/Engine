#pragma once
#include "Engine/Utilities/GameTimer.h"
#include <unordered_map>

//スキル
enum class Skill
{
	kLaunchAttack,
	kSpinAttack,
};

//基本スキルパラメーター
struct SkillParameters
{
	float cooldownDuration = 0.0f; // スキルのクールダウン時間
};

class SkillCooldownManager
{
public:
	void AddSkill(const Skill& skill, const SkillParameters* skillParameters);

	void Update();

	void ResetCooldown(const Skill& skill);

	const bool IsCooldownComplete(const Skill& skill) const;

	const float GetCooldownTime(const Skill& skill) const;

private:
	std::unordered_map<Skill, float> cooldownTimers_{};
	std::unordered_map<Skill, const SkillParameters*> skillParameters_{};
};


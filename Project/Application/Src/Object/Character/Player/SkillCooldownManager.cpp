#include "SkillCooldownManager.h"

void SkillCooldownManager::AddSkill(const Skill& skill, const SkillParameters* skillParameters)
{
	skillParameters_[skill] = skillParameters;
}

void SkillCooldownManager::Update()
{
	//クールダウンタイマーを更新する
	for (auto& [skill, timer] : cooldownTimers_)
	{
		if (timer > 0.0f)
		{
			timer -= GameTimer::GetDeltaTime();
			//タイマーが0未満にならないようにする
			if (timer < 0.0f)
			{
				timer = 0.0f;
			}
		}
	}
}

void SkillCooldownManager::ResetCooldown(const Skill& skill)
{
	auto it = skillParameters_.find(skill);
	if (it != skillParameters_.end())
	{
		cooldownTimers_[skill] = it->second->cooldownDuration;
	}
}

void SkillCooldownManager::ClearAllCooldowns()
{
	for (auto& cooldown : cooldownTimers_)
	{
		cooldown.second = 0.0f;
	}
}

const bool SkillCooldownManager::IsCooldownComplete(const Skill& skill) const
{
	auto it = cooldownTimers_.find(skill);
	if (it != cooldownTimers_.end()) {
		return it->second <= 0.0f;
	}
	return true;
}

const float SkillCooldownManager::GetCooldownTime(const Skill& skill) const
{
	auto it = cooldownTimers_.find(skill);
	if (it != cooldownTimers_.end()) {
		return it->second;
	}
	return 0.0f;
}

#include "SkillCooldownManager.h"

void SkillCooldownManager::AddSkill(const std::string& name, const float cooldownDuration)
{
	skillParameters_[name] = cooldownDuration;
	cooldownTimers_[name] = 0.0f;
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

void SkillCooldownManager::ResetCooldown(const std::string& name)
{
	auto it = skillParameters_.find(name);
	if (it != skillParameters_.end())
	{
		cooldownTimers_[name] = it->second;
	}
}

void SkillCooldownManager::ClearAllCooldowns()
{
	for (auto& cooldown : cooldownTimers_)
	{
		cooldown.second = 0.0f;
	}
}

const bool SkillCooldownManager::IsCooldownComplete(const std::string& name) const
{
	auto it = cooldownTimers_.find(name);
	if (it != cooldownTimers_.end()) 
	{
		return it->second <= 0.0f;
	}
	return true;
}

const float SkillCooldownManager::GetCooldownTime(const std::string& name) const
{
	auto it = cooldownTimers_.find(name);
	if (it != cooldownTimers_.end()) 
	{
		return it->second;
	}
	return 0.0f;
}

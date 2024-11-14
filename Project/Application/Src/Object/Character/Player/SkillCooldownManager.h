#pragma once
#include "Engine/Utilities/GameTimer.h"
#include <unordered_map>
#include <string>

/// <summary>
/// スキルのクールタイムを管理するクラス
/// </summary>
class SkillCooldownManager
{
public:
	/// <summary>
	/// スキルを追加
	/// </summary>
	/// <param name="name">名前</param>
	/// <param name="cooldownDuration">クールタイム時間</param>
	void AddSkill(const std::string& name, const float cooldownDuration);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// クールタイムをリセット
	/// </summary>
	/// <param name="name">名前</param>
	void ResetCooldown(const std::string& name);

	/// <summary>
	/// 全てのスキルのクールタイムをクリア
	/// </summary>
	void ClearAllCooldowns();

	/// <summary>
	/// クールダウンが終了したかどうか
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>クールダウンが終了したかどうか</returns>
	const bool IsCooldownComplete(const std::string& name) const;

	/// <summary>
	/// クールダウンの時間を取得
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>クールダウンの時間</returns>
	const float GetCooldownTime(const std::string& name) const;

private:
	std::unordered_map<std::string, float> cooldownTimers_{};
	std::unordered_map<std::string, float> skillParameters_{};
};


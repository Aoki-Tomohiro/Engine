#pragma once
#include "IPlayerState.h"

/// <summary>
/// アビリティ状態
/// </summary>
class PlayerStateAbility : public IPlayerState
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 衝突処理
	/// </summary>
	/// <param name="other">衝突相手</param>
	void OnCollision(GameObject* other) override;

private:
	/// <summary>
	/// アビリティの名前の取得とフラグのリセット
	/// </summary>
	const std::string GetAbilityNameAndResetFlag() const;

private:
	//アビリティの名前
	std::string abilityName_{};
};


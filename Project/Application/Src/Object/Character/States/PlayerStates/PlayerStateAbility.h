/**
 * @file PlayerStateAbility.h
 * @brief プレイヤーのアビリティ状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "IPlayerState.h"

class PlayerStateAbility : public IPlayerState
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerStateAbility(const int32_t abilityIndex) : abilityIndex_(abilityIndex) {};

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

	//アビリティのインデックス
	int32_t abilityIndex_ = 0;
};


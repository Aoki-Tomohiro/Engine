/**
 * @file PlayerStateAbility.h
 * @brief プレイヤーのアビリティ状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "AbstractPlayerState.h"

class PlayerStateAbility : public AbstractPlayerState
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerStateAbility(const std::string& abilityName) : abilityName_(abilityName) {};

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
	//アビリティの名前
	std::string abilityName_{};

	//アビリティのインデックス
	int32_t abilityIndex_ = 0;
};


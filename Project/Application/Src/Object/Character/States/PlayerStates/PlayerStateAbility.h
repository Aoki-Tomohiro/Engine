#pragma once
#include "IPlayerState.h"

/// <summary>
/// アビリティ状態
/// </summary>
class PlayerStateAbility : public IPlayerState
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="abilityNum">アビリティの番号</param>
	PlayerStateAbility(const int32_t abilityNum) : abilityNum_(abilityNum) {};

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
	//アビリティの番号
	int32_t abilityNum_ = 0;
};


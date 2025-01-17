/**
 * @file PlayerStateAttack.h
 * @brief プレイヤーの攻撃状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "AbstractPlayerState.h"

class PlayerStateAttack : public AbstractPlayerState
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
	/// 空中か地上かによってアニメーション名を決定
	/// </summary>
	/// <returns>アニメーションの名前</returns>
	std::string DetermineAnimationName() const;

	/// <summary>
	/// 地上のアニメーションの名前を返す
	/// </summary>
	/// <returns>地上のアニメーションの名前</returns>
	const std::string GetGroundAnimationName() const;

	/// <summary>
	/// 空中のアニメーションの名前を返す
	/// </summary>
	/// <returns>空中のアニメーションの名前</returns>
	const std::string GetAerialAnimationName() const;

	/// <summary>
	/// ダッシュ攻撃の処理
	/// </summary>
	void HandleDashAttack();

	/// <summary>
	/// コンボのインデックスの更新
	/// </summary>
	void UpdateComboIndex();

private:
	//コンボのインデックス
	static int32_t comboIndex;

	//アニメーションの名前
	std::string animationName_{};

	//次のコンボが有効かどうか
	bool comboNext_ = false;

	//地上攻撃のアニメーション開始時間
	float groundAttackStartTime_ = 0.35f;

	//空中攻撃のアニメーション開始時間
	float aerialAttackStartTime_ = 0.5f;
};


#pragma once
#include "IPlayerState.h"

/// <summary>
/// 攻撃状態
/// </summary>
class PlayerStateAttack : public IPlayerState
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
	/// 地上のアニメーションの名前を返す
	/// </summary>
	/// <returns>地上のアニメーションの名前</returns>
	const std::string SetGroundAnimationName() const;

	/// <summary>
	/// 空中のアニメーションの名前を返す
	/// </summary>
	/// <returns>空中のアニメーションの名前</returns>
	const std::string SetAerialAnimationName() const;

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
};


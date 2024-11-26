#pragma once
#include "IPlayerState.h"
#include "Application/Src/Object/Magic/Magic.h"

/// <summary>
/// 魔法攻撃状態に遷移
/// </summary>
class PlayerStateMagicAttack : public IPlayerState
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
	/// 攻撃イベントの初期化
	/// </summary>
	/// <param name="attackEvent">攻撃イベント</param>
	/// <param name="animationEventIndex">アニメーションイベントのインデックス</param>
	void InitializeAttackEvent(const AttackEvent* attackEvent, const int32_t animationEventIndex) override;

	/// <summary>
	/// 魔法攻撃の初期化
	/// </summary>
	/// <param name="player">プレイヤー</param>
	/// <returns>溜め魔法攻撃が発生したかどうか</returns>
	bool InitializeMagicAttack(Player* player);

	/// <summary>
	/// 魔法を生成
	/// </summary>
	/// <param name="attackEvent">攻撃イベント</param>
	Magic* CreateMagicObject(const AttackEvent* attackEvent);

	/// <summary>
	/// 魔法の速度を計算
	/// </summary>
	/// <returns>魔法の速度</returns>
	Vector3 CalculateMagicVelocity();

	/// <summary>
	/// ロックオン時の速度を計算
	/// </summary>
	/// <param name="player">プレイヤー</param>
	Vector3 CalculateLockonVelocity(Player* player);

	/// <summary>
	/// ノックバックのパラメーターを生成
	/// </summary>
	/// <param name="attackEvent">攻撃イベント</param>
	KnockbackParameters CreateKnockbackParameters(const AttackEvent* attackEvent);

	/// <summary>
	/// 魔法のトランスフォームを設定
	/// </summary>
	/// <param name="magic">魔法</param>
	void SetMagicTransform(Magic* magic);

private:
	//魔法の種類
	Magic::MagicType magicType_{};

	//溜め魔法攻撃のアニメーション開始時間
	float chargeMagicAttackStartTime_ = 1.0f;
};


#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"

class PlayerStateAirAttack : public IPlayerState
{
public:
	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void OnCollision(GameObject* other);

	void OnCollisionEnter(GameObject* other);

	void OnCollisionExit(GameObject* other);

private:
	//コンボの数
	static const int kComboNum = 3;

	//攻撃フェーズの列挙体
	enum ComboPhase
	{
		kCharge,
		kSwing,
		kRecovery,
	};

	//攻撃用定数
	struct ConstAirAttack
	{
		float chargeTime;     // 溜めの時間
		float swingTime;      // 攻撃振りの時間
		float recoveryTime;   // 硬直時間
		float moveSpeed;      // 移動速度
		float attackInterval; // ヒット間隔
		int32_t maxHitCount;     // ヒット回数
	};

	//攻撃用ワーク
	struct WorkAirAttack
	{
		float attackParameter = 0.0f;    // 攻撃用のパラメーター
		int32_t comboIndex = 0;          // 現在のコンボのインデックス
		int32_t inComboPhase = 0;        // 現在のコンボの中のフェーズ
		bool comboNext = false;          // 次のコンボが有効かどうか
		float hitTimer = 0.0f;           // ヒット用タイマー
		int32_t hitCount = 0;            // ヒット回数
		bool isMovementFinished = false; // 移動が終了したかどうか
		float parryTimer = 0.0f;         // パリィ用のタイマー
	};

	//コンボ定数表
	static const std::array<ConstAirAttack, kComboNum> kConstAttacks_;

private:
	//Input
	Input* input_ = nullptr;

	//空中攻撃用ワーク
	WorkAirAttack workAirAttack_{};
};

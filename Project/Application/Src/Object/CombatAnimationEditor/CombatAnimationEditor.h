#pragma once
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/Vector3.h"
#include <algorithm>
#include <map>
#include <string>
#include <fstream>
#include <Engine/Externals/nlohmann/json.hpp>

//リアクションのタイプ
enum class ReactionType
{
	Flinch,     // のけぞり
	Knockback,  // 強力な攻撃で後方に吹き飛ばされる
};

//攻撃に関する設定をまとめる構造体
struct AttackSettings
{
	float moveSpeed = 0;
	int hitCount = 0;         // ヒット数
	float hitInterval = 0.0f; // 攻撃のヒット間隔
	float damage = 0.0f;      // ダメージ
};

//ヒットボックスの設定
struct Hitbox
{
	Vector3 center{}; // 中心位置
	Vector3 size{};   // サイズ（幅、高さ、奥行き）
};

//ノックバックとその関連設定をまとめる構造体
struct KnockbackSettings
{
	Vector3 knockbackVelocity{};                       // ノックバックの速度
	Vector3 knockbackAcceleration{};                   // ノックバックの加速度
	float knockbackDuration = 0.0f;                    // ノックバックの持続時間
	ReactionType reactionType_ = ReactionType::Flinch; // リアクションのタイプ
};

//攻撃の効果をまとめる構造体
struct EffectSettings
{
	float hitStopDuration = 0.0f;     // ヒットストップの持続時間
	float cameraShakeDuration = 0.0f; // カメラシェイクの持続時間
	Vector3 cameraShakeIntensity{};   // カメラシェイクの強度
};

//フェーズ情報を持つ構造体
struct CombatPhase
{
	std::string name{};                    // フェーズの名前
	float duration = 0.0f;                 // フェーズの持続時間
	AttackSettings attackSettings{};       // 攻撃に関する構造体
	Hitbox hitbox{};                       // 判定に関する構造体
	KnockbackSettings knockbackSettings{}; // ノックバックに関する構造体
	EffectSettings effectSettings{};       // エフェクトに関する構造体
};

//アニメーション状態とその関連フェーズを管理する構造体
struct CombatAnimationState
{
	std::string name{};
	std::vector<CombatPhase> phases{};
	char nameBuffer[128]{};
};

class CombatAnimationEditor
{
public:	
	void Initialize();

	void Update();

	const CombatAnimationState GetAnimationState(const std::string& name) const;

private:
	const std::string kFileName = "Application/Resources/Config/Animation/CombatAnimation.json";

	void LoadConfigFile();

	void SaveConfigFile();

	const std::string ReactionTypeToString(const ReactionType& type) const;

	const ReactionType StringToReactionType(const std::string& str) const;

private:
	//アニメーション状態とそれに関連するフェーズを管理するコンテナ
	std::vector<CombatAnimationState> animationStates_;
};


#pragma once
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/Vector3.h"
#include <unordered_map>

//イベントの種類
enum class EventType
{
	kMove, //移動
};

//イベントの基底構造体
struct BaseEvent
{
	EventType eventType;      // イベントの種類
	float startAnimationTime; // イベントの開始時間
	float endAnimationTime;   // イベントの終了時間
};

//移動のタイプ
enum class MovementType
{
	AddVelocity, // 速度を加算して移動
	Easing,      // イージングを使用した移動
};

//移動イベント
struct MoveEvent : public BaseEvent
{
	MovementType movementType; // 移動の種類
};

//速度を加算する移動のパラメーター
struct AddVelocityParameters : public MoveEvent
{
	Vector3 velocity; // 速度
};

//イージングを使用した移動のパラメーター
struct EasingParameters : public MoveEvent
{
	Vector3 startPosition; // 開始座標
	Vector3 endPosition;   // 終了座標
};

/// <summary>
/// 戦闘のアニメーションを管理するクラス
/// </summary>
class CombatAnimationEditor
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

private:
	//アニメーションイベント
	std::unordered_map<std::string, std::vector<BaseEvent>> animationEvents_{};
};


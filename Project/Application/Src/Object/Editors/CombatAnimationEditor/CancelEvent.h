#pragma once
#include "AnimationEvent.h"

//リアクションの種類
enum class CancelType
{
	kMove,
	kDodge,
	kAttack,
};

//キャンセルイベント
struct CancelEvent : public AnimationEvent
{
	CancelType cancelType = CancelType::kMove; //キャンセルタイプ
};
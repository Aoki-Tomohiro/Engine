/**
 * @file DynamicUI.cpp
 * @brief ダイナミックなUIを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "DynamicUI.h"

void DynamicUI::Update()
{
	//自動減少フラグが有効な場合、経過時間を更新
	if (autoDecrement_)
	{
		elapsedTime_ -= GameTimer::GetDeltaTime();
		elapsedTime_ = std::max<float>(elapsedTime_, 0.0f);
	}

	//現在の進行状況を基にスケールを計算
	scale_.x = std::clamp(baseScale_.x * (elapsedTime_ / timeRemaining_), 0.0f, baseScale_.x);

	//基底クラスの更新
	UIElement::Update();
}
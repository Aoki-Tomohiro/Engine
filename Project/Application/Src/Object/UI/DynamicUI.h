/**
 * @file DynamicUI.h
 * @brief ダイナミックなUIを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "UIElement.h"
#include "Engine/Utilities/GameTimer.h"
#include <algorithm>

class DynamicUI : public UIElement
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="textureName">テクスチャの名前</param>
	/// <param name="position">座標</param>
	/// <param name="scale">スケール</param>
	/// <param name="timeRemaining">残り時間</param>
	/// <param name="anchorPoint">アンカーポイント</param>	
	DynamicUI(const std::string& textureName, const Vector2& position, const Vector2& scale, float timeRemaining, const Vector2& anchorPoint = { 0.5f,0.5f }) :
		UIElement(textureName, position, scale, anchorPoint), baseScale_(scale), timeRemaining_(timeRemaining), elapsedTime_(timeRemaining) {};

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	//残り時間を取得・設定
	const float GetTimeRemaining() const { return timeRemaining_; };
	void SetTimeRemaining(const float timeRemaining) { timeRemaining_ = timeRemaining; };

	//経過時間を取得・設定
	const float GetElapsedTime() const { return elapsedTime_; };
	void SetElapsedTime(const float elapsedTime) { elapsedTime_ = elapsedTime; };

	//自動でゲージを減らすかのフラグを取得・設定
	const bool GetAutoDecrement() const { return autoDecrement_; };
	void SetAutoDecrement(const float autoDecrement) { autoDecrement_ = autoDecrement; };

private:
	//基本のスケール
	Vector2 baseScale_ = { 1.0f, 1.0f };

	//残り時間
	float timeRemaining_ = 1.0f;

	//経過時間
	float elapsedTime_ = 0.0f;

	//自動でゲージを減らすかのフラグ
	bool autoDecrement_ = true;
};


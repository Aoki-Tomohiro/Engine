/**
 * @file Transition.h
 * @brief トランジションを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/2D/Sprite.h"

class Transition
{
public:
	/// <summary>
	/// トランジションのの状態
	/// </summary>
	enum FadeState { In, Out };

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// トランジションの速度を設定
	/// </summary>
	/// <param name="transitionSpeed">トランジションの速度</param>
	void SetTransitionSpeed(const float transitionSpeed) { transitionSpeed_ = transitionSpeed; };

	//トランジションの状態の取得・設定
	const FadeState& GetFadeState() const { return fadeState_; };
	void SetFadeState(const FadeState& fadeState) { fadeState_ = fadeState; };

	//フェードインが完了したかどうかを取得
	const bool GetIsFadeInComplete() const { return isFadeInComplete_; };

	//フェードアウトが完了したかどうかを取得
	const bool GetIsFadeOutComplete() const { return isFadeOutComplete_; };

private:
	/// <summary>
	/// アルファ値の更新
	/// </summary>
	void UpdateAlphaValue();

	/// <summary>
	/// フェードのフラグの更新
	/// </summary>
	void UpdateFadeFlags();

private:
	//トランジションのスプライト
	std::unique_ptr<Sprite> sprite_ = nullptr;

	//トランジションのスプライトの色
	Vector4 spriteColor_ = { 0.0f,0.0f,0.0f,1.0f };

	//トランジションの速度
	float transitionSpeed_ = 0.1f;

	//フェードの状態
	FadeState fadeState_ = FadeState::Out;

	//フェードインが終了しているかどうか
	bool isFadeInComplete_ = false;

	//フェードアウトが終了しているかどうか
	bool isFadeOutComplete_ = false;

	//アルファ値の最低値
	const float kMinAlpha = 0.0f;

	//アルファ価の最大値
	const float kMaxAlpha = 1.0f;
};


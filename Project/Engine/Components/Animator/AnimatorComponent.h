/**
 * @file AnimatorComponent.h
 * @brief アニメーターのコンポーネント
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/3D/Model/Animation.h"
#include "Engine/Components/Base/Component.h"

class AnimatorComponent : public Component
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override {};

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// アニメーションを追加
	/// </summary>
	/// <param name="animationName">アニメーションの名前</param>
	/// <param name="animation">アニメーション</param>
	void AddAnimation(const std::string& animationName, Animation* animation);

	/// <summary>
	/// アニメーションを再生
	/// </summary>
	/// <param name="animationName">アニメーションの名前</param>
	/// <param name="speed">アニメーションの速度</param>
	/// <param name="loop">ループフラグ</param>
	/// <param name="inPlaceAxis">動かす軸</param>
	void PlayAnimation(const std::string& animationName, const float speed, const bool loop, const Vector3& inPlaceAxis = { 1.0f,1.0f,1.0f });

	/// <summary>
	/// アニメーションを停止
	/// </summary>
	void StopAnimation();

	/// <summary>
	/// アニメーションを一時停止
	/// </summary>
	void PauseAnimation();

	/// <summary>
	/// アニメーションを再開
	/// </summary>
	void ResumeAnimation();

	//アニメーションブレンドが終了しているか
	const bool GetIsBlendingCompleted() const;

	//指定したアニメーションが終了しているか
	const bool GetIsAnimationFinished(const std::string& animationName) const;

	//現在のアニメーションが終了しているか
	const bool GetIsAnimationFinished();

	//指定したアニメーションの持続時間を取得
	const float GetAnimationDuration(const std::string& animationName) const;

	//現在のアニメーションの持続時間を取得
	const float GetCurrentAnimationDuration() const;

	//次のアニメーションの持続時間を取得
	const float GetNextAnimationDuration() const;

	//指定したアニメーションの時間を取得・設定
	const float GetAnimationTime(const std::string& animationName) const;
	void SetAnimationTime(const std::string& animationName, const float animationTime);

	//現在のアニメーションの時間を取得・設定
	const float GetCurrentAnimationTime() const;
	void SetCurrentAnimationTime(const float animationTime);

	//次のアニメーションの時間を取得・設定
	const float GetNextAnimationTime() const;
	void SetNextAnimationTime(const float animationTime);

	//指定したアニメーションの速度を取得・設定
	const float GetAnimationSpeed(const std::string& animationName) const;
	void SetAnimationSpeed(const std::string& animationName, const float animationSpeed);

	//現在のアニメーションの速度を取得・設定
	const float GetCurrentAnimationSpeed() const;
	void SetCurrentAnimationSpeed(const float animationSpeed);

	//次のアニメーションの速度を取得・設定
	const float GetNextAnimationSpeed() const;
	void SetNextAnimationSpeed(const float animationSpeed);

	//アニメーションブレンドの有効化を取得・設定
	const bool GetIsDlending() const { return isBlending_; };
	void SetIsBlending(const bool isBlending) { isBlending_ = isBlending; };

	//アニメーションブレンドの時間を取得・設定
	const float GetBlendFactor() const { return blendFactor_; };
	void SetBlendFactor(const float blendFactor) { blendFactor_ = blendFactor; };

	//ブレンドの持続時間を取得・設定
	const float GetBlendDuration() const { return blendDuration_; };
	void SetBlendDuration(const float blendDuration) { blendDuration_ = blendDuration; };

	//指定したアニメーションを取得
	Animation* GetAnimation(const std::string& animationName) const;

	//指定したアニメーションをまとめて取得
	const std::map<std::string, std::unique_ptr<Animation>>& GetAnimations() const { return animations_; };

private:
	//アニメーションのマップ
	std::map<std::string, std::unique_ptr<Animation>> animations_{};

	//現在のアニメーションの名前
	std::string currentAnimation_{};

	//次のアニメーションの名前
	std::string nextAnimation_{};

	//アニメーションブレンドが終了したかどうか
	bool isBlendingCompleted_ = true;

	//アニメーションをブレンドするかどうか
	bool isBlending_ = true;

	//アニメーションのブレンド係数
	float blendFactor_ = 0.0f;

	//アニメーションブレンドの持続時間
	float blendDuration_ = 0.3f;

	//固定する軸
	Vector3 inPlaceAxis_ = { 1.0f,1.0f,1.0f };
};


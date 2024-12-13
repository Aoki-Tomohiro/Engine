/**
 * @file ParticleEmitter.h
 * @brief エミッターを管理するクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Vector4.h"
#include "Engine/Math/Quaternion.h"
#include <string>

class ParticleEmitter
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="name">名前</param>
	/// <param name="lifeTime">寿命</param>
	void Initialize(const std::string& name, const float lifeTime);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	//名前を取得・設定
	const std::string& GetName() const { return name_; }
	void SetName(const std::string& name) { name_ = name; };

	//追従対象を取得・設定
	const Vector3* GetFollowTarget() const { return followTarget_; };
	void SetFollowTarget(const Vector3* followTarget) { followTarget_ = followTarget; };

	//座標を取得・設定
	const Vector3& GetTranslate() const { return translate_; };
	void SetTranslate(const Vector3& translate) { translate_ = translate; };

	//半径を取得・設定
	const float GetRadius() const { return radius_; };
	void SetRadius(const float radius) { radius_ = radius; };

	//生成数を取得・設定
	const uint32_t GetCount() const { return count_; };
	void SetCount(const uint32_t count) { count_ = count; };

	//回転の最小値を取得・設定
	const Vector3& GetRotateMin() const { return rotateMin_; };
	void SetRotateMin(const Vector3& rotateMin) { rotateMin_ = rotateMin; };

	//回転の最大値を取得・設定
	const Vector3& GetRotateMax() const { return rotateMax_; };
	void SetRotateMax(const Vector3& rotateMax) { rotateMax_ = rotateMax; };

	//スケールの最小値を取得・設定
	const Vector3& GetScaleMin() const { return scaleMin_; };
	void SetScaleMin(const Vector3& scaleMin) { scaleMin_ = scaleMin; };

	//スケールの最大値を取得・設定
	const Vector3& GetScaleMax() const { return scaleMax_; };
	void SetScaleMax(const Vector3& scaleMax) { scaleMax_ = scaleMax; };

	//速度の最小値を取得・設定
	const Vector3& GetVelocityMin() const { return velocityMin_; };
	void SetVelocityMin(const Vector3& velocityMin) { velocityMin_ = velocityMin; };

	//速度の最大値を取得・設定
	const Vector3& GetVelocityMax() const { return velocityMax_; };
	void SetVelocityMax(const Vector3& velocityMax) { velocityMax_ = velocityMax; };

	//寿命の最小値を取得・設定
	const float GetLifeTimeMin() const { return lifeTimeMin_; };
	void SetLifeTimeMin(const float lifeTimeMin) { lifeTimeMin_ = lifeTimeMin; };

	//寿命の最大値を取得・設定
	const float GetLifeTimeMax() const { return lifeTimeMax_; };
	void SetLifeTimeMax(const float lifeTimeMax) { lifeTimeMax_ = lifeTimeMax; };

	//色の最小値を取得・設定
	const Vector4& GetColorMin() const { return colorMin_; };
	void SetColorMin(const Vector4& colorMin) { colorMin_ = colorMin; };

	//色の最大値を取得・設定
	const Vector4& GetColorMax() const { return colorMax_; };
	void SetColorMax(const Vector4& colorMax) { colorMax_ = colorMax; };

	//発生間隔を取得・設定
	const float GetFrequency() const { return frequency_; };
	void SetFrequency(const float frequency) { frequency_ = frequency; };

	//進行方向に回転させるかを取得・設定
	const bool GetAlignToDirection() const { return alignToDirection_; };
	void SetAlignToDirection(const bool alignToDirection) { alignToDirection_ = alignToDirection; };

	//寿命に応じて色を変えるかを取得・設定
	const bool GetEnableColorOverLifeTime() const { return enableColorOverLifeTime_; };
	void SetEnableColorOverLifeTime(const bool enableColorOverLifeTime) { enableColorOverLifeTime_ = enableColorOverLifeTime; };

	//目標の色を取得・設定
	const Vector3& GetTargetColor() const { return targetColor_; };
	void SetTargetColor(const Vector3& targetColor) { targetColor_ = targetColor; };

	//寿命に応じて透明度を変えるかを取得・設定
	const bool GetEnableAlphaOverLifeTime() const { return enableAlphaOverLifeTime_; };
	void SetEnableAlphaOverLifeTime(const bool enableAlphaOverLifeTime) { enableAlphaOverLifeTime_ = enableAlphaOverLifeTime; };

	//目標の透明度を取得・設定
	const float GetTargetAlpha() const { return targetAlpha_; };
	void SetTargetAlpha(const float targetAlpha) { targetAlpha_ = targetAlpha; };

	//寿命に応じてサイズを変えるかを取得・設定
	const bool GetEnableSizeOverLifeTime() const { return enableSizeOverLifeTime_; };
	void SetEnableSizeOverLifeTime(const bool enableSizeOverLifeTime) { enableSizeOverLifeTime_ = enableSizeOverLifeTime; };

	//目標のスケールを取得・設定
	const Vector3& GetTargetScale()const { return targetScale_; };
	void SetTargetScale(const Vector3& targetScale) { targetScale_ = targetScale; };

	//寿命に応じて回転させるかを取得・設定
	const bool GetEnableRotationOverLifeTime()const { return enableRotationOverLifeTime_; };
	void SetEnableRotationOverLifeTime(const bool enableRotationOverLifeTime) { enableRotationOverLifeTime_ = enableRotationOverLifeTime; };

	//回転速度を取得・設定
	const Vector3& GetRotSpeed() const { return rotSpeed_; };
	void SetRotSpeed(const Vector3& rotSpeed) { rotSpeed_ = rotSpeed; };

	//ビルボードさせるかを取得・設定
	const bool GetIsBillboard() const { return isBillboard_; };
	void SetIsBillboard(const bool isBillboard) { isBillboard_ = isBillboard; };

	//死亡フラグを取得・設定
	const bool GetIsDead() const { return isDead_; };
	void SetIsDead(const bool isDead) { isDead_ = isDead; };

	//クォータニオンを設定
	const Quaternion& GetQuaternion() const { return quaternion_; };

	//パーティクルが発生したかを取得
	const uint32_t GetEmit() const { return emit_; };

private:
	//名前
	std::string name_{};

	//追従座標
	const Vector3* followTarget_ = nullptr;

	//位置
	Vector3 translate_ = { 0.0f,0.0f,0.0f };

	//射出半径
	float radius_ = 1.0f;

	//射出数
	uint32_t count_ = 10;

	//射出許可
	uint32_t emit_ = 0;

	//回転角
	Vector3 rotateMin_ = { 0.0f,0.0f,0.0f };
	Vector3 rotateMax_ = { 0.0f,0.0f,0.0f };

	//Quaternion
	Quaternion quaternion_ = { 0.0f,0.0f,0.0f,1.0f };

	//スケール
	Vector3 scaleMin_ = { 0.2f,0.2f,0.2f };
	Vector3 scaleMax_ = { 0.4f,0.4f,0.4f };

	//速度
	Vector3 velocityMin_ = { -0.2f,-0.2f,-0.2f };
	Vector3 velocityMax_ = { 0.2f,0.2f,0.2f };

	//寿命
	float lifeTimeMin_ = 0.6f;
	float lifeTimeMax_ = 1.0f;

	//色
	Vector4 colorMin_ = { 0.0f,0.0f,0.0f,1.0f };
	Vector4 colorMax_ = { 1.0f,1.0f,1.0f,1.0f };

	//発射間隔
	float frequency_ = 0.5f;

	//射出間隔調整用時間
	float frequencyTime_ = 0.0f;

	//エミッターの寿命
	float emitterLifeTime_ = 1.0f;

	//死亡までのタイマー
	float lifeTimer_ = 0.0f;

	//パーティクルを進行方向に向かせるか
	bool alignToDirection_ = false;

	//寿命に変化に応じて色を変えるかどうか
	bool enableColorOverLifeTime_ = false;

	//目標の色
	Vector3 targetColor_ = { 1.0f,1.0f,1.0f };

	//寿命の変化に応じて透明度を変えるかどうか
	bool enableAlphaOverLifeTime_ = true;

	//目標のアルファ
	float targetAlpha_ = 0.0f;

	//寿命の変化に応じてサイズを変えるかどうか
	bool enableSizeOverLifeTime_ = false;

	//目標のサイズ
	Vector3 targetScale_ = { 0.0f,0.0f,0.0f };

	//寿命の変化に応じて回転させるかどうか
	bool enableRotationOverLifeTime_ = false;

	//回転速度
	Vector3 rotSpeed_ = { 0.0f,0.0f,0.0f };

	//ビルボードフラグ
	bool isBillboard_ = true;

	//死亡フラグ
	bool isDead_ = false;

	//Builderをフレンドクラスに登録
	friend class EmitterBuilder;
};


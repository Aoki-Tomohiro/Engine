#pragma once
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Vector4.h"
#include "Engine/Math/Quaternion.h"
#include <string>

class ParticleEmitter
{
public:
	void Initialize(const std::string& name, const float lifeTime);

	void Update();

	const std::string& GetName() const { return name_; }

	void SetName(const std::string& name) { name_ = name; };

	const Vector3* GetFollowTarget() const { return followTarget_; };

	void SetFollowTarget(const Vector3* followTarget) { followTarget_ = followTarget; };

	const Vector3& GetTranslate() const { return translate_; };

	void SetTranslate(const Vector3& translate) { translate_ = translate; };

	const float GetRadius() const { return radius_; };

	void SetRadius(const float radius) { radius_ = radius; };

	const uint32_t GetCount() const { return count_; };

	void SetCount(const uint32_t count) { count_ = count; };

	const Vector3& GetRotateMin() const { return rotateMin_; };

	void SetRotateMin(const Vector3& rotateMin) { rotateMin_ = rotateMin; };

	const Vector3& GetRotateMax() const { return rotateMax_; };

	void SetRotateMax(const Vector3& rotateMax) { rotateMax_ = rotateMax; };

	const Vector3& GetScaleMin() const { return scaleMin_; };

	void SetScaleMin(const Vector3& scaleMin) { scaleMin_ = scaleMin; };

	const Vector3& GetScaleMax() const { return scaleMax_; };

	void SetScaleMax(const Vector3& scaleMax) { scaleMax_ = scaleMax; };

	const Vector3& GetVelocityMin() const { return velocityMin_; };

	void SetVelocityMin(const Vector3& velocityMin) { velocityMin_ = velocityMin; };

	const Vector3& GetVelocityMax() const { return velocityMax_; };

	void SetVelocityMax(const Vector3& velocityMax) { velocityMax_ = velocityMax; };

	const float GetLifeTimeMin() const { return lifeTimeMin_; };

	void SetLifeTimeMin(const float lifeTimeMin) { lifeTimeMin_ = lifeTimeMin; };

	const float GetLifeTimeMax() const { return lifeTimeMax_; };

	void SetLifeTimeMax(const float lifeTimeMax) { lifeTimeMax_ = lifeTimeMax; };

	const Vector4& GetColorMin() const { return colorMin_; };

	void SetColorMin(const Vector4& colorMin) { colorMin_ = colorMin; };

	const Vector4& GetColorMax() const { return colorMax_; };

	void SetColorMax(const Vector4& colorMax) { colorMax_ = colorMax; };

	const float GetFrequency() const { return frequency_; };

	void SetFrequency(const float frequency) { frequency_ = frequency; };

	const bool GetAlignToDirection() const { return alignToDirection_; };

	void SetAlignToDirection(const bool alignToDirection) { alignToDirection_ = alignToDirection; };

	const bool GetEnableColorOverLifeTime() const { return enableColorOverLifeTime_; };

	void SetEnableColorOverLifeTime(const bool enableColorOverLifeTime) { enableColorOverLifeTime_ = enableColorOverLifeTime; };

	const Vector3& GetTargetColor() const { return targetColor_; };

	void SetTargetColor(const Vector3& targetColor) { targetColor_ = targetColor; };

	const bool GetEnableAlphaOverLifeTime() const { return enableAlphaOverLifeTime_; };

	void SetEnableAlphaOverLifeTime(const bool enableAlphaOverLifeTime) { enableAlphaOverLifeTime_ = enableAlphaOverLifeTime; };

	const float GetTargetAlpha() const { return targetAlpha_; };

	void SetTargetAlpha(const float targetAlpha) { targetAlpha_ = targetAlpha; };

	const bool GetEnableSizeOverLifeTime() const { return enableSizeOverLifeTime_; };

	void SetEnableSizeOverLifeTime(const bool enableSizeOverLifeTime) { enableSizeOverLifeTime_ = enableSizeOverLifeTime; };

	const Vector3& GetTargetScale()const { return targetScale_; };

	void SetTargetScale(const Vector3& targetScale) { targetScale_ = targetScale; };

	const bool GetEnableRotationOverLifeTime()const { return enableRotationOverLifeTime_; };

	void SetEnableRotationOverLifeTime(const bool enableRotationOverLifeTime) { enableRotationOverLifeTime_ = enableRotationOverLifeTime; };

	const Vector3& GetRotSpeed() const { return rotSpeed_; };

	void SetRotSpeed(const Vector3& rotSpeed) { rotSpeed_ = rotSpeed; };

	const bool GetIsBillboard() const { return isBillboard_; };

	void SetIsBillboard(const bool isBillboard) { isBillboard_ = isBillboard; };

	const bool GetIsDead() const { return isDead_; };

	void SetIsDead(const bool isDead) { isDead_ = isDead; };

	const Quaternion& GetQuaternion() const { return quaternion_; };

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


#pragma once
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Vector4.h"
#include <string>

class ParticleEmitter
{
public:
	void Initialize(const std::string& name, const float lifeTime);

	void Update();

	const std::string& GetName() const { return name_; }

	void SetName(const std::string& name) { name_ = name; }

	const Vector3& GetTranslate() const { return translate_; };

	void SetTranslate(const Vector3& translate) { translate_ = translate; };

	const float GetRadius() const { return radius_; };

	void SetRadius(const float radius) { radius_ = radius; };

	const uint32_t GetCount() const { return count_; };

	void SetCount(const uint32_t count) { count_ = count; };

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

	const bool GetIsDead() const { return isDead_; };

	void SetIsDead(const bool isDead) { isDead_ = isDead; };

	const uint32_t GetEmit() const { return emit_; };

private:
	//名前
	std::string name_{};

	//位置
	Vector3 translate_ = { 0.0f,0.0f,0.0f };

	//射出半径
	float radius_ = 1.0f;

	//射出数
	uint32_t count_ = 10;

	//射出許可
	uint32_t emit_ = 0;

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

	//死亡フラグ
	bool isDead_ = false;

	//死亡までのタイマー
	float deathTimer_ = 0.0f;
};


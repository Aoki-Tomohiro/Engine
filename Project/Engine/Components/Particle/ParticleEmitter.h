#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <memory>
#include <string>

class ParticleEmitter
{
public:
	void Initialize(const std::string& name);

	void Update();

	void SetTranslate(const Vector3& translate) { translate_ = translate; };

	void SetRadius(const float radius) { radius_ = radius; };

	void SetCount(const uint32_t count) { count_ = count; };

	void SetFrequency(const float frequency) { frequency_ = frequency; };

	const std::string& GetName() const { return name_; };

	const UploadBuffer* GetEmitterResource() const { return emitterResource_.get(); };

private:
	//Emitterのリソース
	std::unique_ptr<UploadBuffer> emitterResource_ = nullptr;

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
};


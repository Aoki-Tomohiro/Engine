#pragma once
#include "ParticleEmitter.h"

class EmitterBuilder
{
public:
	EmitterBuilder();

	~EmitterBuilder();

	EmitterBuilder& SetEmitterName(const std::string& name);

	EmitterBuilder& SetCount(const uint32_t count);

	EmitterBuilder& SetFrequency(const float frequency);

	EmitterBuilder& SetEmitterLifeTime(const float emitterLifeTime);

	EmitterBuilder& SetTranslation(const Vector3& translate);

	EmitterBuilder& SetRadius(const float radius);

	EmitterBuilder& SetRotate(const Vector3& min, const Vector3& max);

	EmitterBuilder& SetScale(const Vector3& min, const Vector3& max);

	EmitterBuilder& SetVelocity(const Vector3& min, const Vector3& max);

	EmitterBuilder& SetLifeTime(const float min, const float max);

	EmitterBuilder& SetColor(const Vector4& min, const Vector4& max);

	EmitterBuilder& SetAlignToDirection(const bool alignToDirection);

	EmitterBuilder& SetEnableColorOverLifeTime(const bool enableColorOverLifeTime);

	EmitterBuilder& SetTargetColor(const Vector3& targetColor);

	EmitterBuilder& SetEnableAlphaOverLifeTime(const bool enableAlphaOverLifeTime);

	EmitterBuilder& SetTargetAlpha(const float targetAlpha);

	EmitterBuilder& SetEnableSizeOverLifeTime(const bool enableSizeOverLifeTime);

	EmitterBuilder& SetTargetScale(const Vector3& targetScale);

	EmitterBuilder& SetEnableRotationOverLifeTime(const bool enableRotationOverLifeTime);

	EmitterBuilder& SetRotSpeed(const Vector3& rotSpeed);

	EmitterBuilder& SetIsBillboard(const bool isBillboard);

	ParticleEmitter* Build();

private:
	//エミッター
	ParticleEmitter* particleEmitter_ = nullptr;
};


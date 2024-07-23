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

	EmitterBuilder& SetScale(const Vector3& min, const Vector3& max);

	EmitterBuilder& SetVelocity(const Vector3& min, const Vector3& max);

	EmitterBuilder& SetLifeTime(const float min, const float max);

	EmitterBuilder& SetColor(const Vector4& min, const Vector4& max);

	ParticleEmitter* Build();

private:
	//エミッター
	ParticleEmitter* particleEmitter_ = nullptr;
};


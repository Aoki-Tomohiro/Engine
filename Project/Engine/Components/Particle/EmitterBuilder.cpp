#include "EmitterBuilder.h"

EmitterBuilder::EmitterBuilder()
{
	particleEmitter_ = new ParticleEmitter();
}

EmitterBuilder::~EmitterBuilder()
{
}

EmitterBuilder& EmitterBuilder::SetEmitterName(const std::string& name)
{
	particleEmitter_->name_ = name;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetCount(const uint32_t count)
{
	particleEmitter_->count_ = count;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetFrequency(const float frequency)
{
	particleEmitter_->frequency_ = frequency;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetEmitterLifeTime(const float emitterLifeTime)
{
	particleEmitter_->emitterLifeTime_ = emitterLifeTime;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetTranslation(const Vector3& translate)
{
	particleEmitter_->translate_ = translate;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetRadius(const float radius)
{
	particleEmitter_->radius_ = radius;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetScale(const Vector3& min, const Vector3& max)
{
	particleEmitter_->scaleMin_ = min;
	particleEmitter_->scaleMax_ = max;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetVelocity(const Vector3& min, const Vector3& max)
{
	particleEmitter_->velocityMin_ = min;
	particleEmitter_->velocityMax_ = max;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetLifeTime(const float min, const float max)
{
	particleEmitter_->lifeTimeMin_ = min;
	particleEmitter_->lifeTimeMax_ = max;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetColor(const Vector4& min, const Vector4& max)
{
	particleEmitter_->colorMin_ = min;
	particleEmitter_->colorMax_ = max;
	return *this;
}

ParticleEmitter* EmitterBuilder::Build()
{
	return particleEmitter_;
}
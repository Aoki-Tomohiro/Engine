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

EmitterBuilder& EmitterBuilder::SetFollowTarget(const Vector3* followTarget)
{
	particleEmitter_->followTarget_ = followTarget;
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

EmitterBuilder& EmitterBuilder::SetRotate(const Vector3& min, const Vector3& max)
{
	particleEmitter_->rotateMin_ = min;
	particleEmitter_->rotateMax_ = max;
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

EmitterBuilder& EmitterBuilder::SetAlignToDirection(const bool alignToDirection)
{
	particleEmitter_->alignToDirection_ = alignToDirection;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetEnableColorOverLifeTime(const bool enableColorOverLifeTime)
{
	particleEmitter_->enableColorOverLifeTime_ = enableColorOverLifeTime;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetTargetColor(const Vector3& targetColor)
{
	particleEmitter_->targetColor_ = targetColor;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetEnableAlphaOverLifeTime(const bool enableAlphaOverLifeTime)
{
	particleEmitter_->enableAlphaOverLifeTime_ = enableAlphaOverLifeTime;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetTargetAlpha(const float targetAlpha)
{
	particleEmitter_->targetAlpha_ = targetAlpha;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetEnableSizeOverLifeTime(const bool enableSizeOverLifeTime)
{
	particleEmitter_->enableSizeOverLifeTime_ = enableSizeOverLifeTime;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetTargetScale(const Vector3& targetScale)
{
	particleEmitter_->targetScale_ = targetScale;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetEnableRotationOverLifeTime(const bool enableRotationOverLifeTime)
{
	particleEmitter_->enableRotationOverLifeTime_ = enableRotationOverLifeTime;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetRotSpeed(const Vector3& rotSpeed)
{
	particleEmitter_->rotSpeed_ = rotSpeed;
	return *this;
}

EmitterBuilder& EmitterBuilder::SetIsBillboard(const bool isBillboard)
{
	particleEmitter_->isBillboard_ = isBillboard;
	return *this;
}

ParticleEmitter* EmitterBuilder::Build()
{
	return particleEmitter_;
}
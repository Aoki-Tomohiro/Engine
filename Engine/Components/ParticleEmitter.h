#pragma once
#include "Engine/3D/Model.h"
#include "Engine/Math/AABB.h"
#include "Particle.h"
#include <list>

class ParticleEmitter
{
public:
	struct MinMaxStructVector4
	{
		Vector4 min;
		Vector4 max;
	};

	struct MinMaxStructVector3
	{
		Vector3 min;
		Vector3 max;
	};

	struct MinMaxStructFloat
	{
		float min;
		float max;
	};

	struct AccelerationField
	{
		Vector3 acceleration;//加速度
		AABB area;//範囲
	};

	struct GravityField
	{
		Vector3 targetPosition;//集まる位置
		Vector3 velocity;//速度
		AABB area;//範囲
		AABB deleteArea;//消える範囲
	};

	void Update();

	std::list<std::unique_ptr<Particle>>& GetParticles() { return particles_; };

	const std::string& GetName() { return name_; };

	void SetName(const std::string& name) { name_ = name; };

	bool GetIsDead() const { return isActive_; };

	void SetIsDead() { isActive_ = true; };

	float GetDeleteTime() { return deleteTime_; };

	void SetDeleteTime(float deleteTime) { deleteTime_ = deleteTime; };

	const Vector3& GetTranslation() { return translation_; };

	void SetTranslation(const Vector3& translation) { translation_ = translation; };

	const Vector3& GetMinPopArea() { return popArea_.min; };

	const Vector3& GetMaxPopArea() { return popArea_.max; };

	void SetPopArea(const Vector3& min, const Vector3& max) { popArea_ = { min,max }; };

	const Vector3& GetMinPopRotation() { return popRotation_.min; };

	const Vector3& GetMaxPopRotation() { return popRotation_.max; };

	void SetPopRotation(const Vector3& min, const Vector3& max) { popRotation_ = { min,max }; };

	const Vector3& GetMinPopScale() { return popScale_.min; };

	const Vector3& GetMaxPopScale() { return popScale_.max; };

	void SetPopScale(const Vector3& min, const Vector3& max) { popScale_ = { min,max }; };

	float GetMinPopAzimuth() { return popAzimuth.min; };

	float GetMaxPopAzimuth() { return popAzimuth.max; };

	void SetPopAzimuth(float min, float max) { popAzimuth = { min,max }; };

	float GetMinPopElevation() { return popElevation.min; };

	float GetMaxPopElevation() { return popElevation.max; };

	void SetPopElevation(float min, float max) { popElevation = { min,max }; };

	const Vector3& GetMinPopVelocity() { return popArea_.min; };

	const Vector3& GetMaxPopVelocity() { return popArea_.max; };

	void SetPopVelocity(const Vector3& min, const Vector3& max) { popArea_ = { min,max }; };

	const Vector4& GetMinPopColor() { return popColor_.min; };

	const Vector4& GetMaxPopColor() { return popColor_.max; };

	void SetPopColor(const Vector4& min, const Vector4& max) { popColor_ = { min,max }; };

	float GetMinPopLifeTime() { return popLifeTime_.min; };

	float GetMaxPopLifeTime() { return popLifeTime_.max; };

	void SetPopLifeTime(float min, float max) { popLifeTime_ = { min,max }; };

	uint32_t GetPopCount() { return popCount_; };

	void SetPopCount(uint32_t count) { popCount_ = count; };

	float GetPopFrequency() { return popFrequency_; };

	void SetPopFrequency(float frequency) { popFrequency_ = frequency; };

	const Vector3& GetAcceleration() { return accelerationField_.acceleration; };

	const Vector3& GetMinAccelerationField() { return accelerationField_.area.min; };

	const Vector3& GetMaxAccelerationField() { return accelerationField_.area.max; };

	void SetAccelerationField(const Vector3& acceleration, const AABB& area) { accelerationField_ = { acceleration,area }; };

	const Vector3& GetGravityFieldTargetPosition() const { return gravityField_.targetPosition; };

	const Vector3& GetGravityFieldVelocity() const { return gravityField_.velocity; };

	const Vector3& GetMinGravityField() { return gravityField_.area.min; };

	const Vector3& GetMaxGravityField() { return gravityField_.area.max; };

	void SetGravityField(const Vector3& targetPosition, const Vector3& velocity, const AABB& area) { gravityField_ = { targetPosition,velocity,area }; };

private:
	void Pop();

	bool IsCollision(const AABB& aabb, const Vector3& point);

private:
	std::list<std::unique_ptr<Particle>> particles_{};

	std::string name_ = "nameless";

	MinMaxStructVector3 popArea_ = { {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	MinMaxStructVector3 popRotation_ = { {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	MinMaxStructVector3 popScale_ = { {1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f} };

	MinMaxStructFloat popAzimuth = { 0.0f,360.0f };

	MinMaxStructFloat popElevation = { 0.0f,180.0f };

	MinMaxStructVector3 popVelocity_ = { {1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f} };

	MinMaxStructVector4 popColor_ = { {1.0f,1.0f,1.0f,1.0f},{1.0f,1.0f,1.0f,1.0f} };

	MinMaxStructFloat popLifeTime_ = { 0.5f,1.0f };

	Vector3 translation_ = { 0.0f,0.0f,0.0f };

	uint32_t popCount_ = 1;

	float popFrequency_ = 0.1f;

	float frequencyTime_ = popFrequency_;

	float deleteTime_ = 10.0f;

	float deleteTimer_ = 0.0f;

	bool isActive_ = false;

	bool spawnFinished_ = false;

	AccelerationField accelerationField_{};

	GravityField gravityField_{};

	friend class ParticleEmitterBuilder;
};


#pragma once
#include "Engine/Base/RWStructuredBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Camera/Camera.h"

class ParticleSystem
{
public:
	static const uint32_t kMaxParticles = 1024;

	void Initialize();

	void Update();

	void UpdateEmitter();

	void Draw(const Camera* camera);

	void Clear();

	void SetModel(Model* model) { model_ = model; };

	void SetIsBillboard(const bool isBillboard) { isBillboard_ = isBillboard; };

	RWStructuredBuffer* GetParticleResource() const { return particleResource_.get(); };

	RWStructuredBuffer* GetFreeCounterResource() const { return freeCounterResource_.get(); };

private:
	void UpdatePerViewResource(const Camera* camera);

private:
	std::unique_ptr<RWStructuredBuffer> particleResource_ = nullptr;

	std::unique_ptr<RWStructuredBuffer> freeCounterResource_ = nullptr;

	std::unique_ptr<UploadBuffer> perViewResource_ = nullptr;

	std::unique_ptr<Model> defaultModel_ = nullptr;

	Model* model_ = nullptr;

	bool isBillboard_ = true;
};


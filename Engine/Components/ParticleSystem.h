#pragma once
#include "Engine/Base/StructuredBuffer.h"
#include "Engine/Base/DescriptorHandle.h"
#include "ParticleEmitterBuilder.h"

class ParticleSystem
{
public:
	//パーティクルの最大数
	const uint32_t kMaxInstance = 1000;

	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void AddParticleEmitter(ParticleEmitter* particleEmitter) { particleEmitters_.push_back(std::unique_ptr<ParticleEmitter>(particleEmitter)); };

	ParticleEmitter* GetParticleEmitter(const std::string& name);

	std::list<ParticleEmitter*> GetParticleEmitters(const std::string& name);

	const std::string& GetTexture() const { return model_->textureName_; };

	void SetTexture(const std::string& name) const { model_->textureName_ = name; };

	Model* GetModel() { return model_; };

	void SetModel(Model* model) { model_ = model; };

private:
	void CreateInstancingResource();

	void UpdateInstancingResource(const Camera& camera);

private:
	std::unique_ptr<StructuredBuffer> instancingResource_ = nullptr;

	DescriptorHandle descriptorHandle_{};

	uint32_t numInstance_ = 0;

	std::list<std::unique_ptr<ParticleEmitter>> particleEmitters_{};

	Model* model_ = nullptr;
};


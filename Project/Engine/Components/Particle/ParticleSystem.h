#pragma once
#include "Engine/Base/RWStructuredBuffer.h"
#include "Engine/Base/DescriptorHandle.h"
#include "Engine/3D/Model/ModelManager.h"
#include "ParticleEmitterBuilder.h"

class ParticleSystem
{
public:
	//パーティクルの最大数
	static const uint32_t kMaxInstance = 1024;

	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	void Clear();

	void AddParticleEmitter(ParticleEmitter* particleEmitter) { particleEmitters_.push_back(std::unique_ptr<ParticleEmitter>(particleEmitter)); };

	ParticleEmitter* GetParticleEmitter(const std::string& name);

	std::list<ParticleEmitter*> GetParticleEmitters(const std::string& name);

	const bool GetIsBillBoard() const { return isBillboard_; };

	void SetIsBillBoard(const bool isBillboard) { isBillboard_ = isBillboard; };

	void SetModel(Model* model) { model_ = model; };

	void SetTexture(const std::string& name) { model_ ? model_->GetMaterial(0)->SetTexture(name) : defaultModel_->GetMaterial(0)->SetTexture(name); };

	RWStructuredBuffer* GetInstancingResource() const { return instancingResource_.get(); };

private:
	void CreatePerViewResource();

	void UpdatePerViewResource(const Camera& camera);

private:
	std::unique_ptr<RWStructuredBuffer> instancingResource_ = nullptr;

	std::unique_ptr<UploadBuffer> perViewResource_ = nullptr;

	std::list<std::unique_ptr<ParticleEmitter>> particleEmitters_{};

	std::unique_ptr<Model> defaultModel_ = nullptr;

	Model* model_ = nullptr;

	bool isBillboard_ = true;
};


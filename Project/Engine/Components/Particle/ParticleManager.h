#pragma once
#include "ParticleSystem.h"
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/ComputePSO.h"
#include <map>
#include <string>
#include <algorithm>

class ParticleManager
{
public:
	static ParticleManager* GetInstance();

	static void Destroy();

	static ParticleSystem* Create(const std::string& name);

	void Initialize();

	void Update();

	void Draw();

	void Clear();

	void SetCamera(const Camera* camera) { camera_ = camera; };

private:
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;

	void CreateParticlePipelineState();

	void CreateInitializeParticlePipelineState();

	void CreateEmitParticlePipelineState();

	void CreateUpdateParticlePipelineState();

	ParticleSystem* CreateInternal(const std::string& name);

private:
	static ParticleManager* instance_;

	std::map<std::string, std::unique_ptr<ParticleSystem>> particleSystems_{};

	std::unique_ptr<UploadBuffer> perFrameResource_ = nullptr;

	const Camera* camera_ = nullptr;

	RootSignature particleRootSignature_{};

	RootSignature initializeParticleRootSignature_{};

	RootSignature emitParticleRootSignature_{};

	RootSignature updateParticleRootSignature_{};

	std::vector<std::vector<GraphicsPSO>> particlePipelineStates_{};

	ComputePSO initializeParticlePipelineState_{};

	ComputePSO emitParticlePipelineState_{};

	ComputePSO updateParticlePipelineState_{};
};


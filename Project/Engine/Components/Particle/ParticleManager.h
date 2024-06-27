#pragma once
#include "ParticleSystem.h"
#include <unordered_map>

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

	void CreateGraphicsPipelineState();

	void CreateInitializeComputePipelineState();

private:
	ParticleSystem* CreateInternal(const std::string& name);

private:
	static ParticleManager* instance_;

	RootSignature graphicsRootSignature_{};

	GraphicsPSO graphicsPipelineState{};

	RootSignature initializeComputeRootSignature{};

	ComputePSO initializeComputePipelineState_{};

	std::unordered_map<std::string, std::unique_ptr<ParticleSystem>> particleSystems_;

	const Camera* camera_ = nullptr;
};


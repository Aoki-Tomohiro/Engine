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

private:
	ParticleSystem* CreateInternal(const std::string& name);

private:
	static ParticleManager* instance_;

	RootSignature rootSignature_{};

	GraphicsPSO pipelineState_{};

	std::unordered_map<std::string, std::unique_ptr<ParticleSystem>> particleSystems_;

	const Camera* camera_ = nullptr;
};


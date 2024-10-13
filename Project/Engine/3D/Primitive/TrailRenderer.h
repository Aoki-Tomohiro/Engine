#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/3D/Camera/Camera.h"
#include "Trail.h"
#include <vector>

class TrailRenderer
{
public:
	static TrailRenderer* GetInstance();

	static void Destroy();

	static Trail* CreateTrail(const std::string& name);

	void Initialize();

	void Update();

	void Draw();

	const Camera* GetCamera() const { return camera_; };

	void SetCamera(const Camera* camera) { camera_ = camera; };

private:
	TrailRenderer() = default;
	~TrailRenderer() = default;
	TrailRenderer(const TrailRenderer&) = delete;
	TrailRenderer& operator=(const TrailRenderer&) = delete;

	Trail* CreateTrailInternal(const std::string& name);

private:
	static TrailRenderer* instance_;

	RootSignature rootSignature_{};

	GraphicsPSO pipelineState_{};

	std::unordered_map<std::string, std::unique_ptr<Trail>> trails_;

	const Camera* camera_ = nullptr;
};


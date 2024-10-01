#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/Texture.h"
#include "Engine/3D/Camera/Camera.h"
#include <vector>

class TrailRenderer
{
public:
	static const uint32_t kMaxVertices = 1024;

	static TrailRenderer* GetInstance();

	static void Destroy();

	void Initialize();

	void Update();

	void Draw();

	void SetTexture(const std::string& textureName);

	const Camera* GetCamera() const { return camera_; };

	void SetCamera(const Camera* camera) { camera_ = camera; };

private:
	TrailRenderer() = default;
	~TrailRenderer() = default;
	TrailRenderer(const TrailRenderer&) = delete;
	TrailRenderer& operator=(const TrailRenderer&) = delete;

private:
	static TrailRenderer* instance_;

	RootSignature rootSignature_{};

	GraphicsPSO pipelineState_{};

	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::vector<VertexDataPosUV> vertices_{};

	const Texture* texture_ = nullptr;

	const Camera* camera_ = nullptr;
};


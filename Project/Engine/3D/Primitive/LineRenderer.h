#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/3D/Camera/Camera.h"
#include <vector>

class LineRenderer
{
public:
	static const uint32_t kMaxLines = 1024;

	static LineRenderer* GetInstance();

	static void Destroy();

	void Initialize();

	void AddLine(const Vector3& start, const Vector3& end);

	void Draw();

	const Camera* GetCamera() const { return camera_; };

	void SetCamera(const Camera* camera) { camera_ = camera; };

private:
	LineRenderer() = default;
	~LineRenderer() = default;
	LineRenderer(const LineRenderer&) = delete;
	LineRenderer& operator=(const LineRenderer&) = delete;

private:
	static LineRenderer* instance_;

	RootSignature rootSignature_{};

	GraphicsPSO pipelineState_{};

	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::vector<Vector4> vertices_{};

	const Camera* camera_ = nullptr;
};


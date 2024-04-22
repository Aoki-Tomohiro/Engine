#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <vector>
#include <memory>

class Mesh
{
public:
	void Initialize(const std::vector<VertexDataPosUVNormal>& vertices);

	void Update();

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView_; };

	const size_t GetVerticesSize() const { return vertices_.size(); };

private:
	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::vector<VertexDataPosUVNormal> vertices_;
};


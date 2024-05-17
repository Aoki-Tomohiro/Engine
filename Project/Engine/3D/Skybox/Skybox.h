#pragma once
#include "Engine/Base/Texture.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include "Engine/3D/Model/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include <array>

class Skybox
{
public:
	static const uint32_t kMaxVertices = 24;

	static const uint32_t kMaxIndices = 36;

	static Skybox* Create(const std::string& textureName);

	void Draw(const WorldTransform& worldTransform, const Camera& camera);

	void SetTexture(const std::string& textureName);

private:
	void Initialize(const std::string& textureName);

	void CreateVertexBuffer();

	void CreateIndexBuffer();

	void CreateMaterialResource();

private:
	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	std::unique_ptr<UploadBuffer> indexBuffer_ = nullptr;

	std::unique_ptr<UploadBuffer> materialConstBuffer_ = nullptr;

	std::array<VertexDataPosUV, kMaxVertices> vertices_{};

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::array<uint32_t, kMaxIndices> indices_{};

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	const Texture* texture_ = nullptr;
};


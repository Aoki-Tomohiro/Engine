#pragma once
#include "Engine/Base/Texture.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include <array>

class Skybox
{
public:
	static const uint32_t kMaxVertices = 24;

	static const uint32_t kMaxIndices = 36;

	static Skybox* Create();

	void Draw(const Camera& camera);

	const Vector3& GetScale() const { return worldTransform_.scale_; };

	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; };

	const Vector4& GetColor() const { return color_; };

	void SetColor(const Vector4& color) { color_ = color; };

	void SetTexture(const std::string& textureName);

private:
	void CreateInternal();

	void CreateVertexBuffer();

	void CreateIndexBuffer();

	void CreateMaterialResource();

	void UpdateMaterialResource();

private:
	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	std::unique_ptr<UploadBuffer> indexBuffer_ = nullptr;

	std::unique_ptr<UploadBuffer> materialConstBuffer_ = nullptr;

	std::array<VertexDataPosUV, kMaxVertices> vertices_{};

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::array<uint32_t, kMaxIndices> indices_{};

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	WorldTransform worldTransform_{};

	Vector4 color_{ 1.0f,1.0f,1.0f,1.0f };

	const Texture* texture_ = nullptr;
};


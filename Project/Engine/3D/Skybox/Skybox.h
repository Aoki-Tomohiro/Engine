#pragma once
#include "Engine/Base/GraphicsPSO.h"
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

	static Skybox* GetInstance();

	static void Destroy();

	void Initialize();

	void Update();

	void Draw();

	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; };

	void SetRotation(const Vector3& rotation) { worldTransform_.rotation_ = rotation; };

	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; };

	void SetColor(const Vector4& color) { color_ = color; };

	void SetCamera(const Camera* camera) { camera_ = camera; };

	void SetTexture(const std::string& textureName);

private:
	Skybox() = default;
	~Skybox() = default;
	Skybox(const Skybox&) = delete;
	Skybox& operator=(const Skybox&) = delete;

	void CreatePipelineState();

	void CreateVertexBuffer();

	void CreateIndexBuffer();

	void CreateMaterialResource();

	void UpdateMaterialResource();

private:
	static Skybox* instance_;

	RootSignature rootSignature_{};

	GraphicsPSO pipelineState_{};

	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	std::unique_ptr<UploadBuffer> indexBuffer_ = nullptr;

	std::unique_ptr<UploadBuffer> materialConstBuffer_ = nullptr;

	std::array<VertexDataPosUV, kMaxVertices> vertices_{};

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::array<uint32_t, kMaxIndices> indices_{};

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	WorldTransform worldTransform_{};

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	const Camera* camera_ = nullptr;

	const Texture* texture_ = nullptr;
};


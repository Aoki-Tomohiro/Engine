#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include "Engine/Base/Texture.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include <algorithm>

class Trail
{
public:
	static const uint32_t kMaxTrails = 1024;

	struct TrailData
	{
		Vector3 headPosition{};
		Vector3 frontPosition{};
		float dissipationTime = 0.0f;
	};

	void Initialize();

	void Update();

	void AddVertex(const Vector3& head, const Vector3& front);

	void SetColor(const Vector4& color) { color_ = color; };

	void SetNumSegments(const int32_t numSegments) { numSegments_ = numSegments; };

	void SetDissipationDuration(const float dissipationDuration) { dissipationDuration_ = dissipationDuration; };

	const Texture* GetTexture() const { return texture_; };

	void SetTexture(const std::string& textureName);

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView_; };

	const size_t GetNumVertices() const { return trailVertices_.size(); };

	const UploadBuffer* GetMaterialResource() const { return materialResource_.get(); };

private:
	void CreateVertexBuffer();

	void CreateMaterialResource();

	void UpdateTrailData();

	void GenerateTrailVertices();

	void CalculateTrailVertices(const int32_t trailIndex);

	const std::vector<TrailData> GetControlPoints(const int32_t trailIndex);

	void AddTrailVertexData(const Vector3& point, const Vector2& texcoord);

	void UpdateVertexBuffer(const std::vector<TrailData>& trailDatas);

	void UpdateMaterialResource();

private:
	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	std::unique_ptr<UploadBuffer> materialResource_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::vector<TrailData> trailDatas_{};

	std::vector<VertexDataPosUV> trailVertices_{};

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	float dissipationDuration_ = 0.2f;

	int32_t numSegments_ = 10;

	const Texture* texture_ = nullptr;
};


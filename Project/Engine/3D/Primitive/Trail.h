#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include "Engine/Base/Texture.h"
#include "Engine/Math/MathFunction.h"
#include <algorithm>

class Trail
{
public:
	static const uint32_t kMaxTrails = 1024;

	void Initialize();

	void Update();

	void AddVertex(const Vector3& start, const Vector3& end);

	void SetNumSegments(const int32_t numSegments) { numSegments_ = numSegments; };

	const Texture* GetTexture() const { return texture_; };

	void SetTexture(const std::string& textureName);

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView_; };

	const size_t GetNumVertices() const { return trailVertices_.size(); };

private:
	void CalculateTrailVertices(const int32_t vertexIndex);

	const std::vector<Vector3> GetControlPoints(const std::vector<Vector3>& vertices, const int32_t controlVertex);

	void AddTrailVertexData(const Vector3& point, const Vector2& texcoord);

	void UpdateVertexBuffer(const std::vector<VertexDataPosUV>& vertices);

private:
	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::vector<Vector3> startVertices_{};

	std::vector<Vector3> endVertices_{};

	std::vector<VertexDataPosUV> trailVertices_{};

	int32_t numSegments_ = 20;

	const Texture* texture_ = nullptr;
};


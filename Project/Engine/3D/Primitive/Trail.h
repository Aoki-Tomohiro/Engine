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
	//軌跡の最大数
	static const uint32_t kMaxTrails = 1024;

	//軌跡データ
	struct TrailData
	{
		Vector3 headPosition{};
		Vector3 frontPosition{};
		float liefTime = 0.0f;
	};

	void Initialize();

	void Update();

	void AddTrail(const Vector3& head, const Vector3& front);

	void SetStartColor(const Vector4& startColor) { startColor_ = startColor; };

	void SetEndColor(const Vector4& endColor) { endColor_ = endColor; };

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

	void UpdateTrailDatas();

	void GenerateTrailVertices();

	void CalculateTrailVertices(const int32_t trailIndex);

	const std::vector<TrailData> GetControlPoints(const int32_t trailIndex);

	void AddTrailVertexData(const Vector3& point, const Vector2& texcoord);

	void UpdateVertexBuffer();

	void UpdateMaterialResource();

private:
	//頂点バッファ
	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	//マテリアル用のリソース
	std::unique_ptr<UploadBuffer> materialResource_ = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//軌跡の頂点データ
	std::vector<VertexDataPosUV> trailVertices_{};

	//軌跡のデータ
	std::vector<TrailData> trailDatas_{};

	//軌跡の始まりの色
	Vector4 startColor_ = { 1.0f,1.0f,1.0f,1.0f };

	//軌跡の終わりの色
	Vector4 endColor_ = { 1.0f,1.0f,1.0f,1.0f };

	//消えるまでの時間
	float dissipationDuration_ = 0.2f;

	//分割数
	int32_t numSegments_ = 3;

	//テクスチャ
	const Texture* texture_ = nullptr;
};


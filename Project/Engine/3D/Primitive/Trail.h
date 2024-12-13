/**
 * @file Trail.h
 * @brief 軌跡を管理するファイル
 * @author 青木智滉
 * @date
 */

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
	static const uint32_t kMaxTrails = 4096;

	//軌跡データ
	struct TrailData
	{
		Vector3 headPosition{};
		Vector3 frontPosition{};
		float lifeTime = 0.0f;
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	//軌跡を追加
	void AddTrail(const Vector3& head, const Vector3& front);

	//軌跡の開始地点の色を設定
	void SetStartColor(const Vector4& startColor) { startColor_ = startColor; };

	//軌跡の狩猟地点の色を設定
	void SetEndColor(const Vector4& endColor) { endColor_ = endColor; };

	//分割数を設定
	void SetNumSegments(const int32_t numSegments) { numSegments_ = numSegments; };

	//軌跡が消えるまでの時間を設定
	void SetDissipationDuration(const float dissipationDuration) { dissipationDuration_ = dissipationDuration; };

	//テクスチャを設定・取得
	const Texture* GetTexture() const { return texture_; };
	void SetTexture(const std::string& textureName);

	//頂点バッファビューを取得
	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView_; };

	//頂点数を取得
	const size_t GetNumVertices() const { return trailVertices_.size(); };

	//マテリアルリソースを取得
	const UploadBuffer* GetMaterialResource() const { return materialResource_.get(); };

private:
	/// <summary>
	/// 頂点バッファを作成
	/// </summary>
	void CreateVertexBuffer();

	/// <summary>
	/// マテリアル用のリソースを作成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// 軌跡データを更新
	/// </summary>
	void UpdateTrailDatas();

	/// <summary>
	/// 軌跡の頂点を生成
	/// </summary>
	void GenerateTrailVertices();

	/// <summary>
	/// 制御点を取得
	/// </summary>
	/// <param name="trailIndex">軌跡のインデックス</param>
	/// <returns>制御点の配列</returns>
	const std::vector<TrailData> GetControlPoints(const int32_t trailIndex);

	/// <summary>
	/// 軌跡の頂点データを追加
	/// </summary>
	/// <param name="point">制御点</param>
	/// <param name="texcoord">UV座標</param>
	void AddTrailVertexData(const Vector3& point, const Vector2& texcoord);

	/// <summary>
	/// 頂点バッファを更新
	/// </summary>
	void UpdateVertexBuffer();

	/// <summary>
	/// マテリアル用のリソースを更新
	/// </summary>
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
	int32_t numSegments_ = 6;

	//テクスチャ
	const Texture* texture_ = nullptr;
};


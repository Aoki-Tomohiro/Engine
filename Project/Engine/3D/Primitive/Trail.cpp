#include "Trail.h"
#include "Engine/Base/TextureManager.h"

void Trail::Initialize()
{
    //頂点バッファの作成
    CreateVertexBuffer();

    //マテリアル用のリソースの作成
    CreateMaterialResource();

	//デフォルトのテクスチャを設定
    TextureManager::Load("white.png");
	SetTexture("white.png");
}

void Trail::Update()
{
    //頂点データをクリア
    trailVertices_.clear();

    //軌跡データの更新
    UpdateTrailData();

    //有効な軌跡データが足りない場合は終了
    if (trailDatas_.size() < 3) return;

    //頂点データの更新
    GenerateTrailVertices();

    //頂点バッファを更新
    UpdateVertexBuffer(trailDatas_);

    //マテリアル用のリソースの更新
    UpdateMaterialResource();
}

void Trail::AddVertex(const Vector3& head, const Vector3& front)
{
    //新しい軌跡を追加
    TrailData trailData{};
    trailData.headPosition = head;
    trailData.frontPosition = front;
    trailData.dissipationTime = dissipationDuration_;
    trailDatas_.push_back(trailData);
}

void Trail::SetTexture(const std::string& textureName)
{
	//テクスチャを設定
	texture_ = TextureManager::GetInstance()->FindTexture(textureName);
	//テクスチャがなかったら止める
	assert(texture_);
}

void Trail::CreateVertexBuffer()
{
    //頂点バッファの作成
    vertexBuffer_ = std::make_unique<UploadBuffer>();
    vertexBuffer_->Create(sizeof(VertexDataPosUV) * kMaxTrails * 2);

    //頂点バッファビューの作成
    vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
    vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUV);
    vertexBufferView_.SizeInBytes = UINT(sizeof(VertexDataPosUV) * kMaxTrails * 2);
}

void Trail::CreateMaterialResource()
{
    //マテリアル用のリソースの作成
    materialResource_ = std::make_unique<UploadBuffer>();
    materialResource_->Create(sizeof(Vector4));

    //リソースに書き込む
    UpdateMaterialResource();
}

void Trail::UpdateTrailData()
{
    //全ての軌跡データの生存時間を減少させ、時間切れのデータを削除
    for (int32_t i = (int32_t)trailDatas_.size() - 1; i >= 0; --i)
    {
        trailDatas_[i].dissipationTime -= GameTimer::GetDeltaTime();
        if (trailDatas_[i].dissipationTime < 0.0f)
        {
            trailDatas_.erase(trailDatas_.begin() + i);
        }
    }
}

void Trail::GenerateTrailVertices()
{
    //各軌跡の頂点を計算
    for (int32_t i = 0; i < trailDatas_.size() - 1; ++i)
    {
        CalculateTrailVertices(i);
    }
}

void Trail::CalculateTrailVertices(const int32_t trailIndex)
{
    //セグメントごとの計算
    for (int32_t j = 0; j < numSegments_; ++j)
    {
        //セグメント内の進行度を計算
        float t = static_cast<float>(j) / static_cast<float>(numSegments_);

        //制御点を取得
        std::vector<TrailData> trailData = GetControlPoints(trailIndex);

        //Catmull-Romスプラインを用いてポイントを計算
        Vector3 headPoint = Mathf::CatmullRomSpline(trailData[0].headPosition, trailData[1].headPosition, trailData[2].headPosition, trailData[3].headPosition, t);
        Vector3 frontPoint = Mathf::CatmullRomSpline(trailData[0].frontPosition, trailData[1].frontPosition, trailData[2].frontPosition, trailData[3].frontPosition, t);

        //テクスチャ座標を計算
        float texCoordX = (static_cast<float>(trailIndex) + t) / static_cast<float>(trailDatas_.size() - 1);

        //新しい頂点データを追加
        AddTrailVertexData(headPoint, { texCoordX, 0.0f });
        AddTrailVertexData(frontPoint, { texCoordX, 1.0f });
    }
}

const std::vector<Trail::TrailData> Trail::GetControlPoints(const int32_t trailIndex)
{
    //制御点を取得
    std::vector<TrailData> controlPoints(4);
    for (int32_t l = 0; l < controlPoints.size(); ++l)
    {
        //制御点のインデックスを計算
        int32_t vertexIndex = trailIndex - 1 + l;

        //最初の制御点の場合の特別処理
        if (trailIndex == 0)
        {
            vertexIndex = (l == 0) ? trailIndex : vertexIndex;
        }
        //最後の制御点の場合の特別処理
        else if (trailIndex == trailDatas_.size() - 2)
        {
            vertexIndex = (l == controlPoints.size() - 1) ? trailIndex - 2 + l : vertexIndex;
        }
        controlPoints[l] = trailDatas_[vertexIndex];
    }
    return controlPoints;
}

void Trail::AddTrailVertexData(const Vector3& point, const Vector2& texcoord)
{
    //新しい頂点データを作成
    VertexDataPosUV vertex{};
    vertex.position = { point.x, point.y, point.z, 1.0f };
    vertex.texcoord = { texcoord.x, texcoord.y };
    trailVertices_.push_back(vertex);
}

void Trail::UpdateVertexBuffer(const std::vector<TrailData>& trailDatas)
{
    //頂点バッファにデータを書き込む
    VertexDataPosUV* vertexData = static_cast<VertexDataPosUV*>(vertexBuffer_->Map());
    std::memcpy(vertexData, trailVertices_.data(), sizeof(VertexDataPosUV) * trailVertices_.size());
    vertexBuffer_->Unmap();
}

void Trail::UpdateMaterialResource()
{
    //マテリアル用のリソースにデータを書き込む
    Vector4* materialData = static_cast<Vector4*>(materialResource_->Map());
    *materialData = color_;
    materialResource_->Unmap();
}
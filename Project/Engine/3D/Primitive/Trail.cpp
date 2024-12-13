/**
 * @file Trail.cpp
 * @brief 軌跡を管理するファイル
 * @author 青木智滉
 * @date
 */

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
    UpdateTrailDatas();

    //軌跡の頂点データの生成
    GenerateTrailVertices();

    //頂点バッファを更新
    UpdateVertexBuffer();

    //マテリアル用のリソースの更新
    UpdateMaterialResource();
}

void Trail::AddTrail(const Vector3& head, const Vector3& front)
{
    //新しい軌跡を追加
    TrailData trailData{};
    trailData.headPosition = head;
    trailData.frontPosition = front;
    trailData.lifeTime = dissipationDuration_;
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
    vertexBuffer_->Create(sizeof(VertexDataPosUV) * kMaxTrails);

    //頂点バッファビューの作成
    vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
    vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUV);
    vertexBufferView_.SizeInBytes = UINT(sizeof(VertexDataPosUV) * kMaxTrails);
}

void Trail::CreateMaterialResource()
{
    //マテリアル用のリソースの作成
    materialResource_ = std::make_unique<UploadBuffer>();
    materialResource_->Create(sizeof(ConstBuffDataTrailMaterial));

    //リソースに書き込む
    UpdateMaterialResource();
}

void Trail::UpdateTrailDatas()
{
    //全ての軌跡データの生存時間を減少させ、時間切れのデータを削除
    for (int32_t i = (int32_t)trailDatas_.size() - 1; i >= 0; --i)
    {
        trailDatas_[i].lifeTime -= GameTimer::GetDeltaTime();

        if (trailDatas_[i].lifeTime < 0.0f)
        {
            trailDatas_.erase(trailDatas_.begin() + i);
        }
    }
}

void Trail::GenerateTrailVertices()
{
    //制御点が足りない場合は処理を飛ばす
    if (trailDatas_.size() < 3)
    {
        return;
    }

    //各軌跡の頂点を計算
    for (int32_t i = 1; i < trailDatas_.size() - 2; ++i)
    {
        //制御点を取得
        std::vector<TrailData> trailData = GetControlPoints(i);

        //セグメントごとの計算
        for (int32_t j = 0; j < numSegments_; ++j)
        {
            //セグメント中の進行度を計算
            float t = static_cast<float>(j) / static_cast<float>(numSegments_);

            //Catmull-Romスプライン補間を用いて座標を計算
            Vector3 headPosition = Mathf::CatmullRomSpline(trailData[0].headPosition, trailData[1].headPosition, trailData[2].headPosition, trailData[3].headPosition, t);
            Vector3 frontPosition = Mathf::CatmullRomSpline(trailData[0].frontPosition, trailData[1].frontPosition, trailData[2].frontPosition, trailData[3].frontPosition, t);

            //テクスチャ座標を計算
            float texcoordX = (static_cast<float>(i - 1) + t) / static_cast<float>(trailDatas_.size() - 3);

            //新しい頂点データを追加
            AddTrailVertexData(headPosition, { texcoordX, 0.0f });
            AddTrailVertexData(frontPosition, { texcoordX, 1.0f });
        }
    }
}

const std::vector<Trail::TrailData> Trail::GetControlPoints(const int32_t trailIndex)
{
    //制御点
    std::vector<TrailData> controlPoints(4);
    controlPoints[0] = trailDatas_[trailIndex - 1];
    controlPoints[1] = trailDatas_[trailIndex];
    controlPoints[2] = trailDatas_[trailIndex + 1];
    controlPoints[3] = trailDatas_[trailIndex + 2];
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

void Trail::UpdateVertexBuffer()
{
    //頂点バッファにデータを書き込む
    VertexDataPosUV* vertexData = static_cast<VertexDataPosUV*>(vertexBuffer_->Map());
    std::memcpy(vertexData, trailVertices_.data(), sizeof(VertexDataPosUV) * trailVertices_.size());
    vertexBuffer_->Unmap();
}

void Trail::UpdateMaterialResource()
{
    //マテリアル用のリソースにデータを書き込む
    ConstBuffDataTrailMaterial* materialData = static_cast<ConstBuffDataTrailMaterial*>(materialResource_->Map());
    materialData->startColor = startColor_;
    materialData->endColor = endColor_;
    materialResource_->Unmap();
}
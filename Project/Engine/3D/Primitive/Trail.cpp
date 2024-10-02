#include "Trail.h"
#include "Engine/Base/TextureManager.h"

void Trail::Initialize()
{
	//頂点バッファの作成
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sizeof(VertexDataPosUV) * kMaxTrails * 2);

	//頂点バッファビューの作成
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
	vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUV);
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexDataPosUV) * kMaxTrails * 2);

	//デフォルトのテクスチャを設定
    TextureManager::Load("Gradation2.png");
	SetTexture("Gradation2.png");
}

void Trail::Update()
{
    //軌跡の頂点データをクリア
    trailVertices_.clear();

    //頂点を計算
    for (int32_t i = 0; i < startVertices_.size() - 1; ++i)
    {
        CalculateTrailVertices(i);
    }

    //頂点バッファを更新
    UpdateVertexBuffer(trailVertices_);
}

void Trail::AddVertex(const Vector3& start, const Vector3& end)
{
	//始点座標を設定
    startVertices_.push_back(start);

	//終点座標を設定
    endVertices_.push_back(end);
}

void Trail::SetTexture(const std::string& textureName)
{
	//テクスチャを設定
	texture_ = TextureManager::GetInstance()->FindTexture(textureName);
	//テクスチャがなかったら止める
	assert(texture_);
}

void Trail::CalculateTrailVertices(const int32_t vertexIndex)
{
    //セグメントごとの計算
    for (int32_t j = 0; j < numSegments_; ++j)
    {
        //セグメント内の進行度を計算
        float t = static_cast<float>(j) / static_cast<float>(numSegments_);

        //制御点を取得
        std::vector<Vector3> headP = GetControlPoints(startVertices_, vertexIndex);
        std::vector<Vector3> frontP = GetControlPoints(endVertices_, vertexIndex);

        //Catmull-Romスプラインを用いてポイントを計算
        Vector3 headPoint = Mathf::CatmullRomSpline(headP[0], headP[1], headP[2], headP[3], t);
        Vector3 frontPoint = Mathf::CatmullRomSpline(frontP[0], frontP[1], frontP[2], frontP[3], t);

        //テクスチャ座標を計算
        float texCoordX = (static_cast<float>(vertexIndex) + t) / static_cast<float>(startVertices_.size() - 1);

        //新しい頂点データを追加
        AddTrailVertexData(headPoint, { texCoordX, 0.0f });
        AddTrailVertexData(frontPoint, { texCoordX, 1.0f });
    }
}

const std::vector<Vector3> Trail::GetControlPoints(const std::vector<Vector3>& vertices, const int32_t controlVertex)
{
    //制御点の座標を初期化
    std::vector<Vector3> controlPoints(4);
    for (int32_t l = 0; l < controlPoints.size(); ++l)
    {
        //制御点のインデックスを計算
        int32_t vertexIndex = controlVertex - 1 + l;

        //最初の制御点の場合の特別処理
        if (controlVertex == 0)
        {
            vertexIndex = (l == 0) ? controlVertex : vertexIndex;
        }
        //最後の制御点の場合の特別処理
        else if (controlVertex == vertices.size() - 2)
        {
            vertexIndex = (l == controlPoints.size() - 1) ? controlVertex - 2 + l : vertexIndex;
        }
        controlPoints[l] = { vertices[vertexIndex].x, vertices[vertexIndex].y, vertices[vertexIndex].z };
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

void Trail::UpdateVertexBuffer(const std::vector<VertexDataPosUV>& vertices)
{
    //頂点バッファにデータを書き込む
    VertexDataPosUV* vertexData = static_cast<VertexDataPosUV*>(vertexBuffer_->Map());
    std::memcpy(vertexData, vertices.data(), sizeof(VertexDataPosUV) * vertices.size());
    vertexBuffer_->Unmap();
}
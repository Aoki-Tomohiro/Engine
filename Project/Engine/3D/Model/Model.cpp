#include "Model.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Math/MathFunction.h"

void Model::Create(const ModelData& modelData, const Animation& animationData, DrawPass drawPass)
{
	//モデルデータを設定
	modelData_ = modelData;

	//アニメーションデータを設定
	animationData_ = animationData;

	//描画パスを設定
	drawPass_ = drawPass;

	//テクスチャを読み込む
	if (modelData_.material.textureFilePath != "")
	{
		TextureManager::Load(modelData_.material.textureFilePath);
		texture_ = TextureManager::GetInstance()->FindTexture(modelData_.material.textureFilePath);
	}
	else
	{
		texture_ = TextureManager::GetInstance()->FindTexture("white.png");
	}

	//頂点バッファの作成
	CreateVertexBuffer();

	//マテリアル用のリソースの作成
	CreateMaterialConstBuffer();
}

void Model::Update(WorldTransform& worldTransform)
{
	animationTime_ += 1.0f / 60.0f;//時刻を進める。1/60で固定してあるが、計測した時間を使って可変フレーム対応する方が望ましい
	animationTime_ = std::fmod(animationTime_, animationData_.duration);//最後までいったら最初からリピート再生。リピートしなくても別にいい
	NodeAnimation& rootNodeAnimation = animationData_.nodeAnimations[modelData_.rootNode.name];
	Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
	Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
	Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
	Matrix4x4 localMatrix = Mathf::MakeAffineMatrix(scale, rotate, translate);
	worldTransform.matWorld_ = localMatrix * worldTransform.matWorld_;
}

void Model::UpdateMaterailConstBuffer()
{
	//行列の作成
	Matrix4x4 uvTransformMatrix = Mathf::MakeScaleMatrix({ uvScale_.x,uvScale_.y,1.0f });
	uvTransformMatrix = uvTransformMatrix * Mathf::MakeRotateZMatrix(uvRotation_);
	uvTransformMatrix = uvTransformMatrix * Mathf::MakeTranslateMatrix({ uvTranslation_.x,uvTranslation_.y,0.0f });

	//マテリアルデータの更新
	ConstBuffDataMaterial* materialData = static_cast<ConstBuffDataMaterial*>(materialConstBuffer_->Map());
	materialData->color = color_;
	materialData->uvTransform = uvTransformMatrix;
	materialData->enableLighting = enableLighting_;
	materialData->diffuseReflectionType = diffuseReflectionType_;
	materialData->specularReflectionType = specularReflectionType_;
	materialData->shininess = shininess_;
	materialData->specularColor = specularColor_;
	materialConstBuffer_->Unmap();
}

void Model::Draw(WorldTransform& worldTransform, const Camera& camera)
{
	//マテリアルの更新
	UpdateMaterailConstBuffer();

	//RootのMatrixを適用
	worldTransform.matWorld_ = modelData_.rootNode.localMatrix * worldTransform.matWorld_;
	worldTransform.TransferMatrix();

	//レンダラーのインスタンスを取得
	Renderer* renderer_ = Renderer::GetInstance();
	//SortObjectの追加
	renderer_->AddObject(vertexBufferView_, materialConstBuffer_->GetGpuVirtualAddress(),
		worldTransform.GetConstantBuffer()->GetGpuVirtualAddress(), camera.GetConstantBuffer()->GetGpuVirtualAddress(),
		texture_->GetSRVHandle(), UINT(modelData_.vertices.size()), drawPass_);
}

void Model::CreateVertexBuffer()
{
	//頂点バッファを作成
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sizeof(VertexDataPosUVNormal) * modelData_.vertices.size());

	//頂点バッファビューを作成
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexDataPosUVNormal) * modelData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUVNormal);

	//頂点バッファにデータを書き込む
	VertexDataPosUVNormal* vertexData = static_cast<VertexDataPosUVNormal*>(vertexBuffer_->Map());
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexDataPosUVNormal) * modelData_.vertices.size());
	vertexBuffer_->Unmap();
}

void Model::CreateMaterialConstBuffer()
{
	//マテリアル用のリソースの作成
	materialConstBuffer_ = std::make_unique<UploadBuffer>();
	materialConstBuffer_->Create(sizeof(ConstBuffDataMaterial));

	//マテリアル用のリソースに書き込む
	UpdateMaterailConstBuffer();
}

void Model::SetTexture(const std::string& textureName)
{
	//テクスチャを設定
	texture_ = TextureManager::GetInstance()->FindTexture(textureName);
	//テクスチャがなかったら止める
	assert(texture_);
}

Vector3 Model::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time)
{
	assert(!keyframes.empty());//キーがないものは返す値が分からないのでダメ
	//キーが2つか、時刻がキーフレーム前なら最初の値とする
	if (keyframes.size() == 1 || time <= keyframes[0].time)
	{
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index)
	{
		size_t nextIndex = index + 1;
		//indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time)
		{
			//範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Mathf::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	return (*keyframes.rbegin()).value;
}

Quaternion Model::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time)
{
	assert(!keyframes.empty());//キーがないものは返す値が分からないのでダメ
	//キーが2つか、時刻がキーフレーム前なら最初の値とする
	if (keyframes.size() == 1 || time <= keyframes[0].time)
	{
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index)
	{
		size_t nextIndex = index + 1;
		//indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time)
		{
			//範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Mathf::Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}
	return (*keyframes.rbegin()).value;
}
#include "Material.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Math/MathFunction.h"

void Material::Initialize(const MaterialData& materialData)
{
	//マテリアル用のリソースの作成
	materialConstBuffer_ = std::make_unique<UploadBuffer>();
	materialConstBuffer_->Create(sizeof(ConstBuffDataMaterial));

	//色を設定
	if (materialData.color != Vector4{ 0.0f,0.0f,0.0f,0.0f })
	{
		color_ = materialData.color;
	}

	//テクスチャを設定
	if (materialData.textureFilePath != "")
	{
		TextureManager::Load(materialData.textureFilePath);
		SetTexture(materialData.textureFilePath);
	}
	else
	{
		SetTexture("white.png");
	}

	//マスクテクスチャを設定
	TextureManager::Load("noise.png");
	SetMaskTexture("noise.png");

	//リソースに書き込む
	Update();
}

void Material::Update()
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
	materialData->environmentCoefficient = environmentCoefficient_;
	materialData->dissolveThreshold = dissolveThreshold_;
	materialData->edgeWidth = edgeWidth_;
	materialData->edgeColor = edgeColor_;
	materialData->receiveShadows = receiveShadows_;
	materialConstBuffer_->Unmap();
}

void Material::SetTexture(const std::string& textureName)
{
	//テクスチャを設定
	texture_ = TextureManager::GetInstance()->FindTexture(textureName);
	//テクスチャがなかったら止める
	assert(texture_);
}

void Material::SetMaskTexture(const std::string& textureName)
{
	//テクスチャを設定
	maskTexture_ = TextureManager::GetInstance()->FindTexture(textureName);
	//テクスチャがなかったら止める
	assert(texture_);
}
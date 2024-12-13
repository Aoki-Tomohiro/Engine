/**
 * @file Material.h
 * @brief マテリアルを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/Texture.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"

class Material
{
public:
	struct MaterialData
	{
		std::string textureFilePath;
		Vector4 color{};
	};

	enum DiffuseReflectionType
	{
		LambertianReflectance,
		HalfLambert,
	};

	enum SpecularReflectionType
	{
		PhongReflectionModel,
		BlinnPhongReflectionModel,
		NoSpecularReflection,
	};	

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="materialData">マテリアルデータ</param>
	void Initialize(const MaterialData& materialData);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	//色を取得・設定
	const Vector4& GetColor() const { return color_; };
	void SetColor(const Vector4& color) { color_ = color; };

	//UVスケールを取得・設定
	const Vector2& GetUVScale() const { return uvScale_; };
	void SetUVScale(const Vector2& uvScale) { uvScale_ = uvScale; };

	//UVの回転を取得・設定
	const float GetUVRotation() const { return uvRotation_; };
	void SetUVRotation(const float uvRotation) { uvRotation_ = uvRotation; };

	//UV座標を取得・設定
	const Vector2& GetUVTranslation() const { return uvTranslation_; };
	void SetUVTranslation(const Vector2& uvTranslation) { uvTranslation_ = uvTranslation; };

	//ライティングするかを取得・設定
	const int32_t& GetEnableLighting() const { return enableLighting_; };
	void SetEnableLighting(const int32_t enableLighting) { enableLighting_ = enableLighting; };

	//拡散反射の種類を取得・設定
	const int32_t& GetDiffuseReflectionType() const { return int32_t(diffuseReflectionType_); };
	void SetDiffuseReflectionType(const int32_t diffuseReflectionType) { diffuseReflectionType_ = DiffuseReflectionType(diffuseReflectionType); };

	//鏡面反射の取得・設定
	const int32_t& GetSpecularReflectionType() const { return int32_t(specularReflectionType_); };
	void SetSpecularReflectionType(const int32_t specularReflectionType) { specularReflectionType_ = SpecularReflectionType(specularReflectionType); };

	//輝度を取得・設定
	const float& GetShininess() const { return shininess_; };
	void SetShininess(const float shininess) { shininess_ = shininess; };

	//鏡面反射の色を取得・設定
	const Vector3& GetSpecularColor() const { return specularColor_; };
	void SetSpecularColor(const Vector3& specularColor) { specularColor_ = specularColor; };

	//周囲の映り込み係数を取得・設定
	const float GetEnvironmentCoefficient() const { return environmentCoefficient_; };
	void SetEnvironmentCoefficient(const float environmentCoefficient) { environmentCoefficient_ = environmentCoefficient; };

	//ディゾルブの閾値を取得・設定
	const float GetDissolveThreshold() const { return dissolveThreshold_; };
	void SetDissolveThreshold(const float dissolveThreshold) { dissolveThreshold_ = dissolveThreshold; };

	//エッジの幅を取得・設定
	const float GetEdgeWidth() const { return edgeWidth_; };
	void SetEdgeWidth(const float edgeWidth) { edgeWidth_ = edgeWidth; };

	//エッジの色を取得・設定
	const Vector3& GetEdgeColor() const { return edgeColor_; };
	void SetEdgeColor(const Vector3& edgeColor) { edgeColor_ = edgeColor; };

	//影を反映するかを取得・設定
	const bool GetReceiveShadows() const { return receiveShadows_; };
	void SetReceiveShadows(const bool receiveShadows) { receiveShadows_ = receiveShadows; };

	//テクスチャを取得・設定
	const Texture* GetTexture() const { return texture_; };
	void SetTexture(const std::string& textureName);

	//マスクテクスチャを取得・設定
	const Texture* GetMaskTexture() const { return maskTexture_; };
	void SetMaskTexture(const std::string& textureName);

	//コンスタントバッファを取得
	const UploadBuffer* GetConstantBuffer() const { return materialConstBuffer_.get(); };

private:
	std::unique_ptr<UploadBuffer> materialConstBuffer_ = nullptr;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	Vector2 uvScale_ = { 1.0f,1.0f };

	float uvRotation_ = 0.0f;

	Vector2 uvTranslation_ = { 0.0f,0.0f };

	int32_t enableLighting_ = true;

	DiffuseReflectionType diffuseReflectionType_ = DiffuseReflectionType::HalfLambert;

	SpecularReflectionType specularReflectionType_ = SpecularReflectionType::BlinnPhongReflectionModel;

	float shininess_ = 40.0f;

	Vector3 specularColor_ = { 1.0f,1.0f,1.0f };

	float environmentCoefficient_ = 0.2f;

	float dissolveThreshold_ = 0.0f;

	float edgeWidth_ = 0.03f;

	Vector3 edgeColor_ = { 1.0f,0.4f,0.3f };

	bool receiveShadows_ = true;

	const Texture* texture_ = nullptr;

	const Texture* maskTexture_ = nullptr;
};


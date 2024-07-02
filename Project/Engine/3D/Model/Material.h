#pragma once
#include "Engine/Base/Texture.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include "Engine/Math/Vector2.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Vector4.h"

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

	void Initialize(const MaterialData& materialData);

	void Update();

	const Vector4& GetColor() const { return color_; };

	void SetColor(const Vector4& color) { color_ = color; };

	const Vector2& GetUVScale() const { return uvScale_; };

	void SetUVScale(const Vector2& uvScale) { uvScale_ = uvScale; };

	const float GetUVRotation() const { return uvRotation_; };

	void SetUVRotation(const float uvRotation) { uvRotation_ = uvRotation; };

	const Vector2& GetUVTranslation() const { return uvTranslation_; };

	void SetUVTranslation(const Vector2& uvTranslation) { uvTranslation_ = uvTranslation; };

	const int32_t& GetEnableLighting() const { return enableLighting_; };

	void SetEnableLighting(const int32_t enableLighting) { enableLighting_ = enableLighting; };

	const int32_t& GetDiffuseReflectionType() const { return int32_t(diffuseReflectionType_); };

	void SetDiffuseReflectionType(const int32_t diffuseReflectionType) { diffuseReflectionType_ = DiffuseReflectionType(diffuseReflectionType); };

	const int32_t& GetSpecularReflectionType() const { return int32_t(specularReflectionType_); };

	void SetSpecularReflectionType(const int32_t specularReflectionType) { specularReflectionType_ = SpecularReflectionType(specularReflectionType); };

	const float& GetShininess() const { return shininess_; };

	void SetShininess(const float shininess) { shininess_ = shininess; };

	const Vector3& GetSpecularColor() const { return specularColor_; };

	void SetSpecularColor(const Vector3& specularColor) { specularColor_ = specularColor; };

	const float GetEnvironmentCoefficient() const { return environmentCoefficient_; };

	void SetEnvironmentCoefficient(const float environmentCoefficient) { environmentCoefficient_ = environmentCoefficient; };

	const float GetDissolveThreshold() const { return dissolveThreshold_; };

	void SetDissolveThreshold(const float dissolveThreshold) { dissolveThreshold_ = dissolveThreshold; };

	const float GetEdgeWidth() const { return edgeWidth_; };

	void SetEdgeWidth(const float edgeWidth) { edgeWidth_ = edgeWidth; };

	const Vector3& GetEdgeColor() const { return edgeColor_; };

	void SetEdgeColor(const Vector3& edgeColor) { edgeColor_ = edgeColor; };

	const Texture* GetTexture() const { return texture_; };

	void SetTexture(const std::string& textureName);

	const Texture* GetMaskTexture() const { return maskTexture_; };

	void SetMaskTexture(const std::string& textureName);

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

	float environmentCoefficient_ = 0.1f;

	float dissolveThreshold_ = 0.0f;

	float edgeWidth_ = 0.03f;

	Vector3 edgeColor_ = { 1.0f,0.4f,0.3f };

	const Texture* texture_ = nullptr;

	const Texture* maskTexture_ = nullptr;
};


#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Vector4.h"
#include <memory>

class DirectionalLight
{
public:
	enum LightingMethod
	{
		LambertianReflectance,
		HalfLambert
	};

	void Initialize();

	void Update();

	UploadBuffer* GetConstantBuffer() const { return constBuff_.get(); };

	const int32_t& GetEnableLighting() const { return enableLighting_; };

	void SetEnableLighting(const int32_t& enableLighting) { enableLighting_ = enableLighting; };

	const int32_t& GetLightingMethod() const { return int32_t(lightingMethod_); };

	void SetLightingMethod(const int32_t& lightingMethod) { lightingMethod_ = LightingMethod(lightingMethod); };

	const Vector4& GetColor() const { return color_; };

	void SetColor(const Vector4& color) { color_ = color; };

	const Vector3& GetDirection() const { return direction_; };

	void SetDirection(const Vector3& direction) { direction_ = direction; };

	const float& GetIntensity() const { return intensity_; };

	void SetIntensity(const float& intensity) { intensity_ = intensity; };

private:
	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

	int32_t enableLighting_ = true;

	LightingMethod lightingMethod_ = LightingMethod::HalfLambert;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	Vector3 direction_ = { 0.0f,-1.0f,0.0f };

	float intensity_ = 1.0f;
};


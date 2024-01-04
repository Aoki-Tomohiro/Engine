#pragma once
#include "Engine/Math/Vector2.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Vector4.h"
#include "Engine/Math/Matrix4x4.h"
#include <cstdint>

struct VertexDataPosUVNormal 
{
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct VertexDataPosUV
{
	Vector4 position;
	Vector2 texcoord;
};

struct ConstBuffDataMaterial
{
	Vector4 color;
	Matrix4x4 uvTransform;
};

struct ConstBuffDataWorldTransform
{
	Matrix4x4 world;
};

struct ConstBuffDataCamera 
{
	Matrix4x4 view;
	Matrix4x4 projection;
};

struct ConstBuffDataDirectionalLight 
{
	int32_t enableLighting;
	int32_t lightingMethod;
	float padding[2];
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct ParticleForGPU
{
	Matrix4x4 world;
	Vector4 color;
};

struct ConstBuffDataGaussianBlur
{
	int32_t textureWidth;
	int32_t textureHeight;
	int32_t padding[2];
	float weight[8];
};

struct ConstBuffDataBloom
{
	bool enable;
	float intensity;
	float highIntensityColorWeight;
	float highIntensityBlurColorWeight;
	float highIntensityShrinkBlurColorWeight;
};

struct ConstBuffDataFog
{
	//フラグ
	bool enable;
	//スケール
	float scale;
	//減衰率
	float attenuationRate;
};

struct ConstBuffDataDoF
{
	//フラグ
	bool enable;
	float padding[3];
};

struct ConstBuffDataLensDistortion
{
	//フラグ
	bool enable;
	//歪みのきつさ
	float tightness;
	//歪みの強さ
	float strength;
};

struct ConstBuffDataVignette
{
	//フラグ
	bool enable;
	//強度
	float intensity;
};
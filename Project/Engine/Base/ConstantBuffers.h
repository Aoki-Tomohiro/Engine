/**
 * @file ConstantBuffers.h
 * @brief コンスタントバッファの構造体を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Math/Vector2.h"
#include "Engine/Math/Vector3.h"
#include "Engine/Math/Vector4.h"
#include "Engine/Math/Quaternion.h"
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
	int32_t enableLighting;
	int32_t diffuseReflectionType;
	int32_t specularReflectionType;
	float shininess;
	Vector3 specularColor;
	float environmentCoefficient;
	float dissolveThreshold;
	float edgeWidth;
	float padding[2];
	Vector3 edgeColor;
	int32_t receiveShadows;
};

struct ConstBuffDataTrailMaterial
{
	Vector4 startColor;
	Vector4 endColor;
};

enum class RotationType
{
	Euler,
	Quaternion
};

struct ConstBuffDataWorldTransform
{
	Matrix4x4 world;
	Matrix4x4 worldInverseTranspse;
};

struct ConstBuffDataCamera 
{
	Vector3 worldPosition;
	float padding;
	Matrix4x4 view;
	Matrix4x4 projection;
};

struct ConstBuffDataDirectionalLight 
{
	Vector4 color;
	Vector3 direction;
	float intensity;
	int32_t isEnable;
	float padding[3];
};

struct ConstBuffDataPointLight
{
	Vector4 color;
	Vector3 position;
	float intensity;
	float radius;
	float decay;
	int32_t isEnable;
	float padding;
};

struct ConstBuffDataSpotLight
{
	Vector4 color;
	Vector3 position;
	float intensity;
	Vector3 direction;
	float distance;
	float decay;
	float cosFalloffStart;
	float cosAngle;
	int32_t isEnable;
};

struct ParticleForGPU
{
	Matrix4x4 world;
	Vector4 color;
};

struct ParticleCS
{
	Vector3 translate;
	Vector3 rotate;
	Quaternion quaternion;
	Vector3 scale;
	float lifeTime;
	Vector3 velocity;
	float currentTime;
	Vector4 color;
	int32_t alignToDirection;
	Vector3 initialColor;
	Vector3 targetColor;
	float initialAlpha;
	float targetAlpha;
	Vector3 initialScale;
	Vector3 targetScale;
	Vector3 rotSpeed;
	int32_t isBillboard;
};

struct PerView
{
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	Matrix4x4 billboardMatrix;
	Vector3 worldPosition;
};

struct PerFrame
{
	float time;
	float deltaTime;
};

struct EmitterSphere
{
	Vector3 translate;                  //位置
	float radius;                       //射出半径
	uint32_t count;                     //射出数
	uint32_t emit;                      //射出許可
	Vector3 rotateMin;                  //回転角の最小値
	Vector3 rotateMax;                  //回転角の最大値
	Quaternion quaternion;              //Quaternion
	Vector3 scaleMin;                   //スケールの最小値
	Vector3 scaleMax;                   //スケールの最大値
	Vector3 velocityMin;                //速度の最小値
	Vector3 velocityMax;                //速度の最大値
	float lifeTimeMin;                  //寿命の最小値
	float lifeTimeMax;                  //寿命の最大値
	Vector4 colorMin;                   //色の最小値
	Vector4 colorMax;                   //色の最大値
	int32_t alignToDirection;           //パーティクルを進行方向に向かせるか
	int32_t enableColorOverLifeTime;    //パーティクルの寿命に応じて色を変えるかどうか
	Vector3 targetColor;                //目標の色
	int32_t enableAlphaOverLifeTime;    //パーティクルの寿命に応じて透明度を変えるかどうか
	float targetAlpha;                  //目標の透明度
	int32_t enableSizeOverLifeTime;     //パーティクルの寿命に応じて大きさを変えるかどうか
	Vector3 targetScale;                //目標のスケール
	int32_t enableRotationOverLifeTime; //パーティクルの寿命に応じて回転させるかどうか
	Vector3 rotSpeed;                   //各軸の回転速度
	int32_t isBillboard;                //ビルボードフラグ
};

struct AccelerationFieldData
{
	Vector3 acceleration;//加速度
	Vector3 translate;//位置
	Vector3 min;//最小範囲
	Vector3 max;//最大範囲
};

struct GravityFieldData
{
	Vector3 translate;//位置
	Vector3 min;//最小範囲
	Vector3 max;//最大範囲
	float strength;//重力の強さ
	float stopDistance;//動きを止める中心点からの距離
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
	float intensity;
	float textureWeight;
	float highLumTextureWeight;
	float padding;
	float blurTextureWeight[4];
};

struct ConstBuffDataFog
{
	Matrix4x4 projectionInverse;
	Vector3 color;
	float scale;
	float attenuationRate;
};

struct ConstBuffDataDoF
{
	Matrix4x4 projectionInverse;
	float focusDepth;
	float nFocusWidth;
	float fFocusWidth;
};

struct ConstBuffDataLensDistortion
{
	float tightness;
	float strength;
};

struct ConstBuffDataVignette
{
	float scale;
	float intensity;
};

struct ConstBuffDataGrayScale
{
	int32_t isSepiaEnabled;
};

struct ConstBuffDataOutline
{
	Matrix4x4 projectionInverse;
	float coefficient;
};

struct ConstBuffDataRadialBlur
{
	Vector2 center;
	float blurWidth;
};

struct ConstBuffDataHSV
{
	float hue;
	float saturation;
	float value;
};
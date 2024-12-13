/**
 * @file LightManager.h
 * @brief 全てのライトを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include "Engine/Base/Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include <array>
#include <memory>

class LightManager
{
public:
	//DirectionalLightの数
	static const int kNumDirectionalLight = 1;
	//PointLightの数
	static const int kNumPointLight = 1;
	//SpotLightの数
	static const int kNumSpotLight = 1;
	//定数バッファ用構造体
	struct ConstBuffDataLight
	{
		std::array<ConstBuffDataDirectionalLight, kNumDirectionalLight> directionalLights{};
		std::array<ConstBuffDataPointLight, kNumPointLight> pointLights{};
		std::array<ConstBuffDataSpotLight, kNumSpotLight> spotLights{};
	};

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static LightManager* GetInstance();

	/// <summary>
	/// 破棄処理
	/// </summary>
	static void Destroy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	//映り込みのテクスチャを設定・取得
	void SetEnvironmentTexture(const std::string& textureName);
	const Texture* GetEnvironmentTexture() const { return environmentTexture_; };

	//コンスタントバッファを取得
	UploadBuffer* GetConstantBuffer() const { return constBuff_.get(); };

	//平行光源を取得
	DirectionalLight& GetDirectionalLight(uint32_t index) { return directionalLights_[index]; };

	//ポイントライトを取得
	PointLight& GetPointLight(uint32_t index) { return pointLights_[index]; };

	//スポットライトを取得
	SpotLight& GetSpotLight(uint32_t index) { return spotLights_[index]; };

private:
	LightManager() = default;
	~LightManager() = default;
	LightManager(const LightManager&) = delete;
	LightManager& operator=(const LightManager&) = delete;

private:
	static LightManager* instance_;

	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

	std::array<DirectionalLight, kNumDirectionalLight> directionalLights_{};

	std::array<PointLight, kNumPointLight> pointLights_{};

	std::array<SpotLight, kNumSpotLight> spotLights_{};

	const Texture* environmentTexture_ = nullptr;
};


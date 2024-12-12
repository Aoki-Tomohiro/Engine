/**
 * @file Fog.h
 * @brief フォグの管理を行うクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/ColorBuffer.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"

class Fog
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// フォグを適用
	/// </summary>
	/// <param name="srvHandle">Srvハンドル</param>
	void Apply(const DescriptorHandle& srvHandle);

	//有効にするかどうかを設定・取得
	const bool GetIsEnable() const { return isEnable_; };
	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

	//逆プロジェクション行列を取得・設定
	const Matrix4x4& GetProjectionInverse() const { return projectionInverse_; };
	void SetProjectionInverse(const Matrix4x4& projectionInverse) { projectionInverse_ = projectionInverse; };

	//色を取得・設定
	const Vector3& GetColor() const { return color_; };
	void SetColor(const Vector3& color) { color_ = color; };

	//スケールを取得・設定
	const float GetScale() const { return scale_; };
	void SetScale(const float scale) { scale_ = scale; };

	//減衰率を取得・設定
	const float GetAttenuationRate() const { return attenuationRate_; };
	void SetAttenuationRate(const float attenuationRate) { attenuationRate_ = attenuationRate; };

	//デスクリプタハンドルを取得
	const DescriptorHandle& GetDescriptorHandle() const { return colorBuffer_->GetSRVHandle(); };

private:
	/// <summary>
	/// パイプラインステートを生成
	/// </summary>
	void CreatePipelineState();

private:
	//RootSignature
	RootSignature rootSignature_{};

	//PipelineState
	GraphicsPSO pipelineState_{};

	//ColorBuffer
	std::unique_ptr<ColorBuffer> colorBuffer_ = nullptr;

	//ConstBuffer
	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

	//調整項目
	int32_t isEnable_ = false;

	Matrix4x4 projectionInverse_{};

	Vector3 color_ = { 0.6f,0.6f,0.6f };

	float scale_ = 2.0f;

	float attenuationRate_ = 0.0006f;
};


/**
 * @file Outline.h
 * @brief アウトラインを管理するクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/ColorBuffer.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"

class Outline
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
	/// アウトラインを適用
	/// </summary>
	/// <param name="srvHandle">Srvハンドル</param>
	void Apply(const DescriptorHandle& srvHandle);

	//有効にするかどうかを取得・設定
	const bool GetIsEnable() const { return isEnable_; };
	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

	//逆プロジェクション行列を取得・設定
	const Matrix4x4& GetProjectionInverse() const { return projectionInverse_; };
	void SetProjectionInverse(const Matrix4x4& projectionInverse) { projectionInverse_ = projectionInverse; };

	//係数を取得・設定
	const float GetCoefficient() const { return coefficient_; };
	void SetCoefficient(const float coefficient) { coefficient_ = coefficient; };

	//コンスタントバッファを取得
	const UploadBuffer* GetConstBuffer() const { return constBuff_.get(); };

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

	//逆プロジェクション行列
	Matrix4x4 projectionInverse_{};

	//係数
	float coefficient_ = 1.0f;
};


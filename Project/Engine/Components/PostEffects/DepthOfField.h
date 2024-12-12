/**
 * @file DepthOfField.h
 * @brief 被写界深度の管理を行うクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/ColorBuffer.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"

class DepthOfField
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
	/// 被写界深度を適用
	/// </summary>
	/// <param name="srvHandle">Srvハンドル</param>
	void Apply(const DescriptorHandle& srvHandle);

	//有効にするかどうかを取得・設定
	const bool GetIsEnable() const { return isEnable_; };
	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

	//逆プロジェクション行列を取得・設定
	const Matrix4x4& GetProjectionInverse() const { return projectionInverse_; };
	void SetProjectionInverse(const Matrix4x4& projectionInverse) { projectionInverse_ = projectionInverse; };

	//注視する深度を取得・設定
	const float GetFocusDepth() const { return focusDepth_; };
	void SetFocusDepth(const float focusDepth) { focusDepth_ = focusDepth; };

	//注視点から近い深度を取得・設定
	const float GetNFocusWidth() const { return nFocusWidth_; };
	void SetNFocusWidth(const float nFocusWidth) { nFocusWidth_ = nFocusWidth; };

	//注視点から遠い深度を取得・設定
	const float GetFFocusWidth() const { return fFocusWidth_; };
	void SetFFocusWidth(const float fFocusWidth) { fFocusWidth_ = fFocusWidth; };

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

	float focusDepth_ = 0.02f;

	float nFocusWidth_ = 0.005f;

	float fFocusWidth_ = 0.01f;
};


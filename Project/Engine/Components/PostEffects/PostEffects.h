/**
 * @file PostEffects.h
 * @brief ポストエフェクトを管理するクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Bloom.h"
#include "DepthOfField.h"
#include "Fog.h"
#include "LensDistortion.h"
#include "Vignette.h"
#include "GrayScale.h"
#include "Outline.h"
#include "RadialBlur.h"
#include "HSV.h"

class PostEffects
{
public:
	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static PostEffects* GetInstance();

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

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ポストエフェクトを適用
	/// </summary>
	void Apply();

	//裕子王にするかどうかを取得・設定
	const bool GetIsEnable() const { return isEnable_; };
	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

	//被写界深度を取得
	DepthOfField* GetDepthOfField() const { return depthOfField_.get(); };

	//ブルームを取得
	Bloom* GetBloom() const { return bloom_.get(); };

	//フォグを取得
	Fog* GetFog() const { return fog_.get(); };

	//レンズディストーションを取得
	LensDistortion* GetLensDistortion() const { return lensDistortion_.get(); };

	//ビネットを取得
	Vignette* GetVignette() const { return vignette_.get(); };

	//グレースケールを取得
	GrayScale* GetGrayScale() const { return grayScale_.get(); };

	//アウトラインを取得
	Outline* GetOutline() const { return outline_.get(); };

	//ラジアルブラーを取得
	RadialBlur* GetRadialBlur() const { return radialBlur_.get(); };

	//HSVフィルターを取得
	HSV* GetHSV() const { return hsv_.get(); };

private:
	PostEffects() = default;
	~PostEffects() = default;
	PostEffects(const PostEffects&) = delete;
	PostEffects& operator=(const PostEffects&) = delete;

	/// <summary>
	/// 頂点バッファを作成
	/// </summary>
	void CreateVertexBuffer();

	/// <summary>
	/// パイプラインステートを生成
	/// </summary>
	void CreatePipelineState();

private:
	static PostEffects* instance_;

	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	std::array<VertexDataPosUV, 6> vertices_{};

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	RootSignature rootSignature_{};

	GraphicsPSO pipelineState_{};

	std::unique_ptr<DepthOfField> depthOfField_ = nullptr;

	std::unique_ptr<Bloom> bloom_ = nullptr;

	std::unique_ptr<Fog> fog_ = nullptr;

	std::unique_ptr<LensDistortion> lensDistortion_ = nullptr;

	std::unique_ptr<Vignette> vignette_ = nullptr;

	std::unique_ptr<GrayScale> grayScale_ = nullptr;

	std::unique_ptr<Outline> outline_ = nullptr;

	std::unique_ptr<RadialBlur> radialBlur_ = nullptr;

	std::unique_ptr<HSV> hsv_ = nullptr;

	DescriptorHandle currentDescriptorHandle_{};

	bool isEnable_ = false;
};


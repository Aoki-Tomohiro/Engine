#pragma once
#include "Engine/Base/PipelineState.h"
#include "GaussianBlur.h"
#include "Bloom.h"
#include "Fog.h"
#include "DepthofField.h"
#include "LensDistortion.h"
#include "Vignette.h"

class PostProcess
{
public:
	enum BlurType
	{
		kNormal,
		kDownSample,
		kCountOfBlurType
	};

	static const uint32_t kMaxVertices = 6;

	static PostProcess* GetInstance();

	static void Destroy();

	void Initialize();

	void Update();

	void Draw();

	void ApplyPostProcess();

	const bool GetIsEnable() const { return isEnable_; };

	void SetIsEnable(const bool isEnable) { isEnable_ = isEnable; };

	GaussianBlur* GetGaussianBlur(BlurType type) { return gaussianBlurs_[type].get(); };

	Bloom* GetBloom() { return bloom_.get(); };

	Fog* GetFog() { return fog_.get(); }

	DepthofField* GetDepthfoField() { return depthofField_.get(); };

	LensDistortion* GetLensDistortion() { return lensDistortion_.get(); };

	Vignette* GetVignette() { return vignette_.get(); };

private:
	PostProcess() = default;
	~PostProcess() = default;
	PostProcess(const PostProcess&) = delete;
	PostProcess& operator=(const PostProcess&) = delete;

	void CreateColorBuffers();

	void CreateVertexBuffer();

	void CreateMultiPassPipelineState();

	void CreateGaussianBlurPipelineState();

	void CreatePostProcessPipelineState();

	void SecondPassDraw();

	void ApplyGaussianBlur(BlurType type, GaussianBlur::BlurDirection direction);

private:
	static PostProcess* instance_;

	//ColorBuffer
	std::unique_ptr<ColorBuffer> multiPassColorBuffer_ = nullptr;
	std::unique_ptr<ColorBuffer> highLumColorBuffer_ = nullptr;

	//DescriptorHandle
	DescriptorHandle multiPassDescriptorHandle_{};
	DescriptorHandle highLumDescriptorHandle_{};

	//VertexBuffer
	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;
	std::array<VertexDataPosUV, 6> vertices_{};
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//RootSignature
	RootSignature multiPassRootSignature_{};
	RootSignature postProcessRootSignature_{};
	RootSignature gaussianBlurRootSignature_{};

	//PipelineState
	PipelineState multiPassPipelineState_{};
	PipelineState postProcessPipelineState_{};
	std::array<PipelineState, GaussianBlur::kCountOfBlurDirection> gaussianBlurPipelineStates_{};

	//GaussianBlur
	std::array<std::unique_ptr<GaussianBlur>, kCountOfBlurType> gaussianBlurs_{};

	//Bloom
	std::unique_ptr<Bloom> bloom_ = nullptr;

	//Fog
	std::unique_ptr<Fog> fog_ = nullptr;

	//DepthofField
	std::unique_ptr<DepthofField> depthofField_ = nullptr;

	//LensDistortion
	std::unique_ptr<LensDistortion> lensDistortion_ = nullptr;

	//Vignette
	std::unique_ptr<Vignette> vignette_ = nullptr;

	//フラグ
	bool isEnable_ = false;
};


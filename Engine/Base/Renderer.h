#pragma once
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "DescriptorHandle.h"
#include "PipelineState.h"
#include "Engine/3D/DirectionalLight.h"
#include <vector>

class Renderer
{
public:
	enum BlendMode {
		//ブレンドなし
		kBlendModeNone,
		//通常aブレンド
		kBlendModeNormal,
		//加算
		kBlendModeAdd,
		//減算
		kBlendModeSubtract,
		//乗算
		kBlendModeMultiply,
		//スクリーン
		kBlendModeScreen,
		//利用してはいけない
		kCountOfBlendMode,
	};

	static Renderer* GetInstance();

	static void Destroy();

	void Initialize();

	void PreDraw();

	void PostDraw();

	void PreDrawModels();

	void PostDrawModels();

	void PreDrawParticles();

	void PostDrawParticles();

	void PreDrawSprites(BlendMode blendMode);

	void PostDrawSprites();

	DescriptorHandle GetSceneColorDescriptorHandle() { return sceneColorDescriptorHandle_; };

	DescriptorHandle GetSceneDepthDescriptorHandle() { return sceneDepthDescriptorHandle_; };

	DirectionalLight* GetDirectionalLight() { return directionalLight_.get(); };

private:
	Renderer() = default;
	~Renderer() = default;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void CreateModelPipelineState();

	void CreateSpritePipelineState();

	void CreateParticlePipelineState();

private:
	static Renderer* instance_;

	std::unique_ptr<ColorBuffer> sceneColorBuffer_ = nullptr;

	std::unique_ptr<DepthBuffer> sceneDepthBuffer_ = nullptr;

	DescriptorHandle sceneColorDescriptorHandle_{};

	DescriptorHandle sceneDepthDescriptorHandle_{};

	std::unique_ptr<DirectionalLight> directionalLight_ = nullptr;

	RootSignature modelRootSignature_{};

	RootSignature spriteRootSignature_{};

	RootSignature particleRootSignature_{};

	std::vector<PipelineState> modelPipelineStates_{};

	std::vector<PipelineState> spritePipelineStates_{};

	std::vector<PipelineState> particlePipelineStates_{};
};


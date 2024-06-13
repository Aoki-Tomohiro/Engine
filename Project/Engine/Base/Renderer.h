#pragma once
#include "Engine/3D/Lights/LightManager.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"
#include "GraphicsPSO.h"
#include "ComputePSO.h"
#include <vector>

enum DrawPass
{
	Opaque,
	Transparent,
	NumTypes,
};

enum BlendMode 
{
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

struct SortObject {
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	D3D12_GPU_VIRTUAL_ADDRESS materialCBV;
	D3D12_GPU_VIRTUAL_ADDRESS worldTransformCBV;
	D3D12_GPU_VIRTUAL_ADDRESS cameraCBV;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSRV;
	D3D12_GPU_DESCRIPTOR_HANDLE matrixPaletteSRV;
	D3D12_GPU_DESCRIPTOR_HANDLE inputVerticesSRV;
	D3D12_GPU_DESCRIPTOR_HANDLE influencesSRV;
	D3D12_GPU_DESCRIPTOR_HANDLE outputVerticesUAV;
	D3D12_GPU_VIRTUAL_ADDRESS skininngInformationCBV;
	UINT indexCount;
	UINT vertexCount;
	DrawPass type;
};

struct DebugObject
{
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_GPU_VIRTUAL_ADDRESS worldTransformCBV;
	D3D12_GPU_VIRTUAL_ADDRESS cameraCBV;
	UINT vertexCount;
};

class Renderer
{
public:
	enum RootBindings 
	{
		//マテリアル
		kMaterial,
		//ワールドトランスフォーム
		kWorldTransform,
		//カメラ
		kCamera,
		//テクスチャ
		kTexture,
		//ライト
		kLight,
		//環境テクスチャ
		kEnvironmentTexture,
	};

	enum ComputeRootBindings
	{
		//MatrixPalette
		kMatrixPalette,
		//InputVertices
		kInputVertices,
		//Influences
		kInfluences,
		//OutputVertices
		kOutputVertices,
		//SkinningInformation
		kSkinningInformation,
	};

	static Renderer* GetInstance();

	static void Destroy();

	void Initialize();

	void AddObject(SortObject& sortObject);

	void AddDebugObject(D3D12_VERTEX_BUFFER_VIEW vertexBufferView,
		D3D12_GPU_VIRTUAL_ADDRESS worldTransformCBV,
		D3D12_GPU_VIRTUAL_ADDRESS cameraCBV,
		UINT indexCount);

	void Render();

	void ClearRenderTarget();

	void ClearDepthBuffer();

	void PreDraw();

	void PostDraw();

	void PreDrawSprites(BlendMode blendMode);

	void PostDrawSprites();

	void PreDrawParticles();

	void PostDrawParticles();

	void PreDrawSkybox();

	void PostDrawSkybox();

	const DescriptorHandle& GetSceneColorDescriptorHandle() const { return sceneColorBuffer_->GetSRVHandle(); };

	const DescriptorHandle& GetLinearDepthDescriptorHandle() const { return linearDepthColorBuffer_->GetSRVHandle(); };

private:
	Renderer() = default;
	~Renderer() = default;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void CreateModelPipelineState();

	void CreateSkinningModelPipelineState();

	void CreateDebugPipelineState();

	void CreateSpritePipelineState();

	void CreateParticlePipelineState();

	void CreateSkyboxPipelineState();

	void Sort();

private:
	static Renderer* instance_;

	std::vector<SortObject> sortObjects_{};

	std::vector<DebugObject> debugObjects_{};

	std::unique_ptr<ColorBuffer> sceneColorBuffer_ = nullptr;

	std::unique_ptr<ColorBuffer> linearDepthColorBuffer_ = nullptr;

	std::unique_ptr<DepthBuffer> sceneDepthBuffer_ = nullptr;

	LightManager* lightManager_ = nullptr;

	RootSignature modelRootSignature_{};

	RootSignature skinningModelRootSignature_{};

	RootSignature skinningModelComputeRootSignature_{};

	RootSignature debugRootSignature_{};

	RootSignature spriteRootSignature_{};

	RootSignature particleRootSignature_{};

	RootSignature skyboxRootSignature_{};

	std::vector<GraphicsPSO> modelPipelineStates_{};

	std::vector<GraphicsPSO> skinningModelPipelineStates_{};

	std::vector<ComputePSO> skinningModelComputePipelineStates_{};

	std::vector<GraphicsPSO> debugPipelineStates_{};

	std::vector<GraphicsPSO> spritePipelineStates_{};

	std::vector<GraphicsPSO> particlePipelineStates_{};

	std::vector<GraphicsPSO> skyboxPipelineStates_{};
};


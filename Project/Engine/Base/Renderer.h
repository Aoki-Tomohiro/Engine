#pragma once
#include "Engine/3D/Lights/LightManager.h"
#include "Engine/3D/Camera/CameraManager.h"
#include "RWStructuredBuffer.h"
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
		//マスクテクスチャ
		kMaskTexture,
		//環境テクスチャ
		kEnvironmentTexture,
		//ライトのカメラ
		kLightCamera,
		//影テクスチャ
		kShadowTexture,
	};

	enum ComputeRootBindings
	{
		kMatrixPalette,
		kInputVertices,
		kInfluences,
		kOutputVertices,
		kSkinningInformation,
	};

	struct ShadowCameraParameters
	{
		float width = 60.0f;
		float height = 60.0f;
		float nearZ = 0.1f;
		float farZ = 100.0f;
	};

	static Renderer* GetInstance();

	static void Destroy();

	void Initialize();

	void AddObject(D3D12_VERTEX_BUFFER_VIEW vertexBufferView,
		D3D12_INDEX_BUFFER_VIEW indexBufferView,
		D3D12_GPU_VIRTUAL_ADDRESS materialCBV,
		D3D12_GPU_VIRTUAL_ADDRESS worldTransformCBV,
		D3D12_GPU_VIRTUAL_ADDRESS cameraCBV,
		D3D12_GPU_DESCRIPTOR_HANDLE textureSRV,
		D3D12_GPU_DESCRIPTOR_HANDLE maskTextureSRV,
		UINT indexCount,
		DrawPass drawPass);

	void AddSkinningObject(D3D12_GPU_DESCRIPTOR_HANDLE matrixPaletteSRV,
		D3D12_GPU_DESCRIPTOR_HANDLE inputVerticesSRV,
		D3D12_GPU_DESCRIPTOR_HANDLE influencesSRV,
		D3D12_GPU_VIRTUAL_ADDRESS skinningInformationCBV,
		RWStructuredBuffer* outpuVerticesBuffer,
		UINT vertexCount);

	void AddShadowObject(D3D12_VERTEX_BUFFER_VIEW vertexBufferView,
		D3D12_INDEX_BUFFER_VIEW indexBufferView,
		D3D12_GPU_VIRTUAL_ADDRESS worldTransformCBV,
		UINT indexCount);

	void AddBone(D3D12_VERTEX_BUFFER_VIEW vertexBufferView,
		D3D12_GPU_VIRTUAL_ADDRESS worldTransformCBV,
		D3D12_GPU_VIRTUAL_ADDRESS cameraCBV,
		UINT indexCount);

	void Render();

	void ClearRenderTarget();

	void ClearDepthBuffer();

	void PreDraw();

	void PostDraw();

	void PreDrawShadow();

	void PostDrawShadow();

	void PreDrawSprites(BlendMode blendMode);

	void PostDrawSprites();

	void PreDrawSkybox();

	void PostDrawSkybox();

	const DescriptorHandle& GetSceneColorDescriptorHandle() const { return sceneColorBuffer_->GetSRVHandle(); };

	const DescriptorHandle& GetSceneDepthDescriptorHandle() const { return sceneDepthBuffer_->GetSRVHandle(); };

	void SetShadowCameraParameters(const ShadowCameraParameters& shadowCameraParameters) { shadowCameraParameters_ = shadowCameraParameters; };

	Camera* GetShadowCamera() const { return shadowCamera_; };

private:
	Renderer() = default;
	~Renderer() = default;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	struct SortObject {
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		D3D12_GPU_VIRTUAL_ADDRESS materialCBV;
		D3D12_GPU_VIRTUAL_ADDRESS worldTransformCBV;
		D3D12_GPU_VIRTUAL_ADDRESS cameraCBV;
		D3D12_GPU_DESCRIPTOR_HANDLE textureSRV;
		D3D12_GPU_DESCRIPTOR_HANDLE maskTextureSRV;
		UINT indexCount;
		DrawPass type;
	};

	struct SkinningObject
	{
		D3D12_GPU_DESCRIPTOR_HANDLE matrixPaletteSRV;
		D3D12_GPU_DESCRIPTOR_HANDLE inputVerticesSRV;
		D3D12_GPU_DESCRIPTOR_HANDLE influencesSRV;
		D3D12_GPU_VIRTUAL_ADDRESS skinningInformationCBV;
		RWStructuredBuffer* outpuVerticesBuffer;
		UINT vertexCount;
	};

	struct ShadowObject
	{
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		D3D12_GPU_VIRTUAL_ADDRESS worldTransformCBV;
		UINT indexCount;
	};

	struct Bone
	{
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		D3D12_GPU_VIRTUAL_ADDRESS worldTransformCBV;
		D3D12_GPU_VIRTUAL_ADDRESS cameraCBV;
		UINT vertexCount;
	};

	void CreateModelPipelineState();

	void CreateSkinningModelPipelineState();

	void CreateBonePipelineState();

	void CreateSpritePipelineState();

	void CreateSkyboxPipelineState();

	void CreateShadowPipelineState();

	void Sort();

private:
	static Renderer* instance_;

	std::vector<SortObject> sortObjects_{};

	std::vector<SkinningObject> skinningObjects_{};

	std::vector<ShadowObject> shadowObjects_{};

	std::vector<Bone> bones_{};

	std::unique_ptr<ColorBuffer> sceneColorBuffer_ = nullptr;

	std::unique_ptr<DepthBuffer> sceneDepthBuffer_ = nullptr;

	std::unique_ptr<DepthBuffer> shadowDepthBuffer_ = nullptr;

	LightManager* lightManager_ = nullptr;

	RootSignature modelRootSignature_{};

	RootSignature skinningModelRootSignature_{};

	RootSignature boneRootSignature_{};

	RootSignature spriteRootSignature_{};

	RootSignature skyboxRootSignature_{};

	RootSignature shadowRootSignature_{};

	std::vector<GraphicsPSO> modelPipelineStates_{};

	std::vector<ComputePSO> skinningModelPipelineStates_{};

	std::vector<GraphicsPSO> bonePipelineStates_{};

	std::vector<GraphicsPSO> spritePipelineStates_{};

	std::vector<GraphicsPSO> skyboxPipelineStates_{};

	std::vector<GraphicsPSO> shadowPipelineStates_{};

	Camera* shadowCamera_ = nullptr;

	ShadowCameraParameters shadowCameraParameters_{};
};


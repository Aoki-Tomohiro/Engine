/**
 * @file Renderer.h
 * @brief 描画を処理するファイル
 * @author 青木智滉
 * @date
 */

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

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static Renderer* GetInstance();

	/// <summary>
	/// 破棄処理
	/// </summary>
	static void Destroy();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// オブジェクトを追加
	/// </summary>
	/// <param name="vertexBufferView">頂点バッファビュー</param>
	/// <param name="indexBufferView">インデックスバッファビュー</param>
	/// <param name="materialCBV">マテリアル用のCBV</param>
	/// <param name="worldTransformCBV">ワールド座標用のCBV</param>
	/// <param name="cameraCBV">カメラ用のCBV</param>
	/// <param name="textureSRV">テクスチャのSRV</param>
	/// <param name="maskTextureSRV">マスクテクスチャのSRV</param>
	/// <param name="indexCount">インデックスの数</param>
	/// <param name="drawPass">描画の種類</param>
	void AddObject(D3D12_VERTEX_BUFFER_VIEW vertexBufferView,
		D3D12_INDEX_BUFFER_VIEW indexBufferView,
		D3D12_GPU_VIRTUAL_ADDRESS materialCBV,
		D3D12_GPU_VIRTUAL_ADDRESS worldTransformCBV,
		D3D12_GPU_VIRTUAL_ADDRESS cameraCBV,
		D3D12_GPU_DESCRIPTOR_HANDLE textureSRV,
		D3D12_GPU_DESCRIPTOR_HANDLE maskTextureSRV,
		UINT indexCount,
		DrawPass drawPass);

	/// <summary>
	/// スキニングオブジェクトを追加
	/// </summary>
	/// <param name="matrixPaletteSRV">マトリックスパレットのSRV</param>
	/// <param name="inputVerticesSRV">入力用の頂点のSRV</param>
	/// <param name="influencesSRV">インフルエンス用のSRV</param>
	/// <param name="skinningInformationCBV">スキニング用のその他のデータのCBV</param>
	/// <param name="outpuVerticesBuffer">出力用の頂点バッファ</param>
	/// <param name="vertexCount">頂点数</param>
	void AddSkinningObject(D3D12_GPU_DESCRIPTOR_HANDLE matrixPaletteSRV,
		D3D12_GPU_DESCRIPTOR_HANDLE inputVerticesSRV,
		D3D12_GPU_DESCRIPTOR_HANDLE influencesSRV,
		D3D12_GPU_VIRTUAL_ADDRESS skinningInformationCBV,
		RWStructuredBuffer* outpuVerticesBuffer,
		UINT vertexCount);

	/// <summary>
	/// 影の描画用のオブジェクトを追加
	/// </summary>
	/// <param name="vertexBufferView">頂点バッファビュー</param>
	/// <param name="indexBufferView">インデックスバッファビュー</param>
	/// <param name="worldTransformCBV">ワールド座標用のCBV</param>
	/// <param name="indexCount">インデックスの数</param>
	void AddShadowObject(D3D12_VERTEX_BUFFER_VIEW vertexBufferView,
		D3D12_INDEX_BUFFER_VIEW indexBufferView,
		D3D12_GPU_VIRTUAL_ADDRESS worldTransformCBV,
		UINT indexCount);

	/// <summary>
	/// ボーンの追加
	/// </summary>
	/// <param name="vertexBufferView">頂点バッファビュー</param>
	/// <param name="worldTransformCBV">ワールド座標用のCBV</param>
	/// <param name="cameraCBV">カメラ用のCBV</param>
	/// <param name="indexCount">インデックスの数</param>
	void AddBone(D3D12_VERTEX_BUFFER_VIEW vertexBufferView,
		D3D12_GPU_VIRTUAL_ADDRESS worldTransformCBV,
		D3D12_GPU_VIRTUAL_ADDRESS cameraCBV,
		UINT indexCount);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Render();

	/// <summary>
	/// レンダーターゲットをクリア
	/// </summary>
	void ClearRenderTarget();

	/// <summary>
	/// 深度バッファをクリア
	/// </summary>
	void ClearDepthBuffer();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 影の描画前処理
	/// </summary>
	void PreDrawShadow();

	/// <summary>
	/// 影の描画後処理
	/// </summary>
	void PostDrawShadow();

	/// <summary>
	/// スプライトの描画前処理
	/// </summary>
	/// <param name="blendMode">ブレンドモード</param>
	void PreDrawSprites(BlendMode blendMode);

	/// <summary>
	/// スプライトの描画後処理
	/// </summary>
	void PostDrawSprites();

	/// <summary>
	/// スカイボックスの描画前処理
	/// </summary>
	void PreDrawSkybox();

	/// <summary>
	/// スカイボックスの描画後処理
	/// </summary>
	void PostDrawSkybox();

	//シーンのカラーバッファのデスクリプタハンドルを取得
	const DescriptorHandle& GetSceneColorDescriptorHandle() const { return sceneColorBuffer_->GetSRVHandle(); };

	//シーンの深度バッファのデスクリプタハンドルを取得
	const DescriptorHandle& GetSceneDepthDescriptorHandle() const { return sceneDepthBuffer_->GetSRVHandle(); };

	//影描画用のカメラパラメーターを設定
	void SetShadowCameraParameters(const ShadowCameraParameters& shadowCameraParameters) { shadowCameraParameters_ = shadowCameraParameters; };

	//影用のカメラを取得
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

	/// <summary>
	/// モデルのパイプラインステートを生成
	/// </summary>
	void CreateModelPipelineState();

	/// <summary>
	/// スキニングモデル用のパイプラインステートを生成
	/// </summary>
	void CreateSkinningModelPipelineState();

	/// <summary>
	/// Bone用のパイプラインステートを生成
	/// </summary>
	void CreateBonePipelineState();

	/// <summary>
	/// スプライトのパイプラインステートを生成
	/// </summary>
	void CreateSpritePipelineState();

	/// <summary>
	/// スカイボックスのパイプラインステートを生成
	/// </summary>
	void CreateSkyboxPipelineState();

	/// <summary>
	/// 影用のパイプラインステートを生成
	/// </summary>
	void CreateShadowPipelineState();

	/// <summary>
	/// 描画順をソート
	/// </summary>
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


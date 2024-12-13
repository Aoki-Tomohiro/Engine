/**
 * @file ParticleManager.cpp
 * @brief 全てのパーティクルシステムを管理するクラス
 * @author 青木智滉
 * @date
 */

#include "ParticleManager.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Utilities/ShaderCompiler.h"
#include "Engine/Utilities/GameTimer.h"

//実体定義
ParticleManager* ParticleManager::instance_ = nullptr;

ParticleManager* ParticleManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new ParticleManager();
	}
	return instance_;
}

void ParticleManager::Destroy()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

ParticleSystem* ParticleManager::Create(const std::string& name)
{
	ParticleSystem* particleSystem = ParticleManager::GetInstance()->CreateInternal(name);
	return particleSystem;
}

void ParticleManager::Initialize()
{
	//PerFrameResourceの作成
	perFrameResource_ = std::make_unique<UploadBuffer>();
	perFrameResource_->Create(sizeof(PerFrame));

	//ParticleのPipelineを作成
	CreateParticlePipelineState();

	//InitializeParticleのPipelineを作成
	CreateInitializeParticlePipelineState();

	//EmitParticleのPipelineStateを作成
	CreateEmitParticlePipelineState();

	//UpdateParticleのPipelineStateを作成
	CreateUpdateParticlePipelineState();

	//デフォルトのパーティクル画像を読み込む
	TextureManager::Load("DefaultParticle.png");
}

void ParticleManager::Update()
{
	//PerFrameResourceの更新
	PerFrame* perFrameData = static_cast<PerFrame*>(perFrameResource_->Map());
	perFrameData->time = GameTimer::GetElapsedTime();
	perFrameData->deltaTime = GameTimer::GetDeltaTime();
	perFrameResource_->Unmap();

	for (auto& particleSystem : particleSystems_)
	{
		//コマンドリストを取得
		CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();

		//DescriptorHeapを設定
		commandContext->SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, GraphicsCore::GetInstance()->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

		//EmitParticleRootSignatureを設定
		commandContext->SetComputeRootSignature(emitParticleRootSignature_);

		//EmitParticlePipelineStateを設定
		commandContext->SetPipelineState(emitParticlePipelineState_);

		//PerFrameを設定
		commandContext->SetComputeConstantBuffer(5, perFrameResource_->GetGpuVirtualAddress());

		//Emitterの更新
		particleSystem.second->UpdateEmitter();

		//UpdateParticleRootSignatureを設定
		commandContext->SetComputeRootSignature(updateParticleRootSignature_);

		//UpdateParticlePipelineStateを設定
		commandContext->SetPipelineState(updateParticlePipelineState_);

		//PerFrameを設定
		commandContext->SetComputeConstantBuffer(7, perFrameResource_->GetGpuVirtualAddress());

		//Particleの更新
		particleSystem.second->Update();
	}
}

void ParticleManager::Draw()
{
	//カメラがない場合は飛ばす
	if (!camera_)
	{
		return;
	};

	//デプスを書き込むパーティクルシステム
	std::vector<ParticleSystem*> depthWriteParticleSystems;
	//デプスを書き込まないパーティクルシステム
	std::vector<ParticleSystem*> depthNoWriteParticleSystems;

	//パーティクルシステムをブレンドモード順にソートするためにベクターに変換
	std::vector<ParticleSystem*> sortedParticleSystems;
	for (const auto& pair : particleSystems_)
	{
		if (pair.second->GetEnableDepthWrite())
		{
			depthWriteParticleSystems.push_back(pair.second.get());
		}
		else
		{
			depthNoWriteParticleSystems.push_back(pair.second.get());
		}
	}

	//パーティクルシステムをブレンドモード順にソート
	std::sort(depthWriteParticleSystems.begin(), depthWriteParticleSystems.end(),
		[](const ParticleSystem* lhs, const ParticleSystem* rhs) {
			return lhs->GetBlendMode() < rhs->GetBlendMode();
		});

	std::sort(depthNoWriteParticleSystems.begin(), depthNoWriteParticleSystems.end(),
		[](const ParticleSystem* lhs, const ParticleSystem* rhs) {
			return lhs->GetBlendMode() < rhs->GetBlendMode();
		});

	//コマンドリストを取得
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();

	//RootSignatureを設定
	commandContext->SetRootSignature(particleRootSignature_);

	//Lightを設定
	LightManager* lightManager = LightManager::GetInstance();
	commandContext->SetConstantBuffer(4, lightManager->GetConstantBuffer()->GetGpuVirtualAddress());

	//現在のブレンドモードを保存
	BlendMode currentBlendMode = kBlendModeNone;

	//パーティクルの描画
	for (auto& particleSystem : depthWriteParticleSystems)
	{
		//パーティクルシステムのブレンドモードを取得
		BlendMode blendMode = particleSystem->GetBlendMode();

		//ブレンドモードが変わったらPipelineStateを変更
		if (blendMode != currentBlendMode)
		{
			//PipelineStateを設定
			commandContext->SetPipelineState(particlePipelineStates_[1][blendMode]);

			//現在のブレンドモードを更新
			currentBlendMode = blendMode;
		}

		//パーティクルの描画
		particleSystem->Draw(camera_);
	}

	//ブレンドモードをリセット
	currentBlendMode = kBlendModeNone;

	//パーティクルの描画
	for (auto& particleSystem : depthNoWriteParticleSystems)
	{
		//パーティクルシステムのブレンドモードを取得
		BlendMode blendMode = particleSystem->GetBlendMode();

		//ブレンドモードが変わったらPipelineStateを変更
		if (blendMode != currentBlendMode)
		{
			//PipelineStateを設定
			commandContext->SetPipelineState(particlePipelineStates_[0][blendMode]);

			//現在のブレンドモードを更新
			currentBlendMode = blendMode;
		}

		//パーティクルの描画
		particleSystem->Draw(camera_);
	}
}

void ParticleManager::Clear()
{
	for (auto& particleSystem : particleSystems_)
	{
		particleSystem.second->Clear();
	}
}

void ParticleManager::CreateParticlePipelineState()
{
	//RootSignatureの作成
	particleRootSignature_.Create(5, 1);
	particleRootSignature_[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	particleRootSignature_[1].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_VERTEX);
	particleRootSignature_[2].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_VERTEX);
	particleRootSignature_[3].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	particleRootSignature_[4].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_PIXEL);

	//StaticSamplerを設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	particleRootSignature_.InitStaticSampler(0, staticSamplers[0], D3D12_SHADER_VISIBILITY_PIXEL);
	particleRootSignature_.Finalize();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc[6]{};
	//ブレンドなし
	blendDesc[0].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//半透明合成
	blendDesc[1].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[1].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[1].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[1].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[1].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[1].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[1].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	blendDesc[1].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値

	//加算合成
	blendDesc[2].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[2].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[2].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[2].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[2].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[2].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[2].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blendDesc[2].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う

	//減算合成
	blendDesc[3].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[3].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[3].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[3].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[3].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[3].RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//減算
	blendDesc[3].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blendDesc[3].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う

	//乗算合成
	blendDesc[4].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[4].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[4].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[4].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[4].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[4].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[4].RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;//ソースの値を0%使う
	blendDesc[4].RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;//デストの色を使う

	//スクリーン合成
	blendDesc[5].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[5].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[5].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[5].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[5].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[5].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[5].RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1-ソースの色を使う
	blendDesc[5].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの色を100%使う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = ShaderCompiler::CompileShader(L"Particle.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = ShaderCompiler::CompileShader(L"Particle.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	std::vector<D3D12_DEPTH_STENCIL_DESC> depthStencilDescs(2);
	//Depthの機能を無効化する
	depthStencilDescs[0].DepthEnable = false;
	//書き込みしない
	depthStencilDescs[0].DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDescs[0].DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//Depthの機能を有効化する
	depthStencilDescs[1].DepthEnable = true;
	//書き込みします
	depthStencilDescs[1].DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDescs[1].DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//書き込むRTVの情報
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//PipelineStateを作成
	for (uint32_t i = 0; i < depthStencilDescs.size(); ++i)
	{
		std::vector<GraphicsPSO> pipelineStatesForCurrentDepth;
		for (uint32_t j = 0; j < kCountOfBlendMode; j++)
		{
			GraphicsPSO pipelineState;
			pipelineState.SetRootSignature(&particleRootSignature_);
			pipelineState.SetInputLayout(3, inputElementDescs);
			pipelineState.SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
			pipelineState.SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
			pipelineState.SetBlendState(blendDesc[j]);
			pipelineState.SetRasterizerState(rasterizerDesc);
			pipelineState.SetRenderTargetFormats(1, &rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);
			pipelineState.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
			pipelineState.SetSampleMask(D3D12_DEFAULT_SAMPLE_MASK);
			pipelineState.SetDepthStencilState(depthStencilDescs[i]);
			pipelineState.Finalize();
			pipelineStatesForCurrentDepth.push_back(pipelineState);
		}
		particlePipelineStates_.push_back(pipelineStatesForCurrentDepth);
	}
}

void ParticleManager::CreateInitializeParticlePipelineState()
{
	//RootSignatureの作成
	initializeParticleRootSignature_.Create(3, 0);
	initializeParticleRootSignature_[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1, D3D12_SHADER_VISIBILITY_ALL);
	initializeParticleRootSignature_[1].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1, D3D12_SHADER_VISIBILITY_ALL);
	initializeParticleRootSignature_[2].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 2, 1, D3D12_SHADER_VISIBILITY_ALL);
	initializeParticleRootSignature_.Finalize();

	//PipelineStateの作成
	Microsoft::WRL::ComPtr<IDxcBlob> computeShaderBlob = ShaderCompiler::CompileShader(L"InitializeParticle.CS.hlsl", L"cs_6_0");
	assert(computeShaderBlob != nullptr);
	initializeParticlePipelineState_.SetRootSignature(&initializeParticleRootSignature_);
	initializeParticlePipelineState_.SetComputeShader(computeShaderBlob->GetBufferPointer(), computeShaderBlob->GetBufferSize());
	initializeParticlePipelineState_.Finalize();
}

void ParticleManager::CreateEmitParticlePipelineState()
{
	//RootSignatureの作成
	emitParticleRootSignature_.Create(6, 0);
	emitParticleRootSignature_[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1, D3D12_SHADER_VISIBILITY_ALL);
	emitParticleRootSignature_[1].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1, D3D12_SHADER_VISIBILITY_ALL);
	emitParticleRootSignature_[2].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 2, 1, D3D12_SHADER_VISIBILITY_ALL);
	emitParticleRootSignature_[3].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_ALL);
	emitParticleRootSignature_[4].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_ALL);
	emitParticleRootSignature_[5].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_ALL);
	emitParticleRootSignature_.Finalize();

	//PipelineStateの作成
	Microsoft::WRL::ComPtr<IDxcBlob> computeShaderBlob = ShaderCompiler::CompileShader(L"EmitParticle.CS.hlsl", L"cs_6_0");
	assert(computeShaderBlob != nullptr);
	emitParticlePipelineState_.SetRootSignature(&emitParticleRootSignature_);
	emitParticlePipelineState_.SetComputeShader(computeShaderBlob->GetBufferPointer(), computeShaderBlob->GetBufferSize());
	emitParticlePipelineState_.Finalize();
}

void ParticleManager::CreateUpdateParticlePipelineState()
{
	//RootSignatureの作成
	updateParticleRootSignature_.Create(8, 0);
	updateParticleRootSignature_[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1, D3D12_SHADER_VISIBILITY_ALL);
	updateParticleRootSignature_[1].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1, D3D12_SHADER_VISIBILITY_ALL);
	updateParticleRootSignature_[2].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 2, 1, D3D12_SHADER_VISIBILITY_ALL);
	updateParticleRootSignature_[3].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_ALL);
	updateParticleRootSignature_[4].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, D3D12_SHADER_VISIBILITY_ALL);
	updateParticleRootSignature_[5].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_ALL);
	updateParticleRootSignature_[6].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_ALL);
	updateParticleRootSignature_[7].InitAsConstantBuffer(2, D3D12_SHADER_VISIBILITY_ALL);
	updateParticleRootSignature_.Finalize();

	//PipelineStateの作成
	Microsoft::WRL::ComPtr<IDxcBlob> computeShaderBlob = ShaderCompiler::CompileShader(L"UpdateParticle.CS.hlsl", L"cs_6_0");
	assert(computeShaderBlob != nullptr);
	updateParticlePipelineState_.SetRootSignature(&updateParticleRootSignature_);
	updateParticlePipelineState_.SetComputeShader(computeShaderBlob->GetBufferPointer(), computeShaderBlob->GetBufferSize());
	updateParticlePipelineState_.Finalize();
}

ParticleSystem* ParticleManager::CreateInternal(const std::string& name)
{
	auto it = particleSystems_.find(name);

	if (it != particleSystems_.end())
	{
		return it->second.get();
	}

	//パーティクルの生成
	ParticleSystem* particleSystem = new ParticleSystem();
	particleSystem->Initialize();
	particleSystems_[name] = std::unique_ptr<ParticleSystem>(particleSystem);

	//コマンドリストを作成
	CommandContext commandContext{};
	commandContext.Initialize();

	//コマンドキューを作成
	CommandQueue commandQueue{};
	commandQueue.Initialize();

	//DescriptorHeapを設定
	commandContext.SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, GraphicsCore::GetInstance()->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	//ParticleResourceを取得
	RWStructuredBuffer* particleResource = particleSystem->GetParticleResource();

	//FreeListIndexResourceを取得
	RWStructuredBuffer* freeListIndexResource = particleSystem->GetFreeListIndexResource();

	//FreeListResourceを取得
	RWStructuredBuffer* freeListResource = particleSystem->GetFreeListResource();

	//ParticleResourceの状態を遷移
	commandContext.TransitionResource(*particleResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	//FreeListIndexResourceの状態を遷移
	commandContext.TransitionResource(*freeListIndexResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	//FreeListResourceの状態を遷移
	commandContext.TransitionResource(*freeListResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	//RootSignatureを設定
	commandContext.SetComputeRootSignature(initializeParticleRootSignature_);

	//PipelineStateを設定
	commandContext.SetPipelineState(initializeParticlePipelineState_);

	//ParticleResourceを設定
	commandContext.SetComputeDescriptorTable(0, particleResource->GetUAVHandle());

	//FreeListIndexResourceを設定
	commandContext.SetComputeDescriptorTable(1, freeListIndexResource->GetUAVHandle());

	//FreeListResourceを設定
	commandContext.SetComputeDescriptorTable(2, freeListResource->GetUAVHandle());

	//Dispatch
	commandContext.Dispatch(1, 1, 1);

	//コマンドリストを閉じる
	commandContext.Close();

	//コマンドを実行
	ID3D12CommandList* commandLists[] = { commandContext.GetCommandList() };
	commandQueue.ExecuteCommandList(commandLists);

	//Fenceを待つ
	commandQueue.WaitForFence();

	//コマンドリストをリセット
	commandContext.Reset();

	return particleSystem;
}
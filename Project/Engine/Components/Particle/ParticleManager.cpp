#include "ParticleManager.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Utilities/ShaderCompiler.h"

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
	if (instance_)
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
	//GraphicsPipelineStateの作成
	CreateGraphicsPipelineState();

	//ComputePipelineStateの作成
	CreateInitializeComputePipelineState();

	//デフォルトのパーティクル画像を読み込む
	TextureManager::Load("DefaultParticle.png");
}

void ParticleManager::Update()
{
	//パーティクルの更新
	for (auto& particleSystem : particleSystems_)
	{
		particleSystem.second->Update();
	}
}

void ParticleManager::Draw()
{
	//コマンドリストを取得
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();

	//RootSignatureを設定
	commandContext->SetRootSignature(graphicsRootSignature_);

	//PipelineStateを設定
	commandContext->SetPipelineState(graphicsPipelineState);

	//パーティクルの描画
	for (auto& particleSystem : particleSystems_)
	{
		assert(camera_);
		particleSystem.second->Draw(*camera_);
	}
}

void ParticleManager::Clear()
{
	for (auto& particleSystem : particleSystems_)
	{
		particleSystem.second->Clear();
	}
}

void ParticleManager::CreateGraphicsPipelineState()
{
	graphicsRootSignature_.Create(4, 1);
	graphicsRootSignature_[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	graphicsRootSignature_[1].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_VERTEX);
	graphicsRootSignature_[2].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_VERTEX);
	graphicsRootSignature_[3].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);

	//StaticSamplerを設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	graphicsRootSignature_.InitStaticSampler(0, staticSamplers[0], D3D12_SHADER_VISIBILITY_PIXEL);
	graphicsRootSignature_.Finalize();

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
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//共通設定
	blendDesc.RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	//加算合成
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = ShaderCompiler::CompileShader(L"Particle.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = ShaderCompiler::CompileShader(L"Particle.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を無効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みしない
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//書き込むRTVの情報
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//PipelineStateを作成
	graphicsPipelineState.SetRootSignature(&graphicsRootSignature_);
	graphicsPipelineState.SetInputLayout(3, inputElementDescs);
	graphicsPipelineState.SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
	graphicsPipelineState.SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
	graphicsPipelineState.SetBlendState(blendDesc);
	graphicsPipelineState.SetRasterizerState(rasterizerDesc);
	graphicsPipelineState.SetRenderTargetFormats(1, &rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);
	graphicsPipelineState.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	graphicsPipelineState.SetSampleMask(D3D12_DEFAULT_SAMPLE_MASK);
	graphicsPipelineState.SetDepthStencilState(depthStencilDesc);
	graphicsPipelineState.Finalize();
}

void ParticleManager::CreateInitializeComputePipelineState()
{
	//RootSignatureの作成
	initializeComputeRootSignature.Create(1, 0);
	initializeComputeRootSignature[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0, 1, D3D12_SHADER_VISIBILITY_ALL);
	initializeComputeRootSignature.Finalize();

	//PipelineStateの作成
	Microsoft::WRL::ComPtr<IDxcBlob> computeShaderBlob = ShaderCompiler::CompileShader(L"InitializeParticle.CS.hlsl", L"cs_6_0");
	assert(computeShaderBlob != nullptr);
	initializeComputePipelineState_.SetRootSignature(&initializeComputeRootSignature);
	initializeComputePipelineState_.SetComputeShader(computeShaderBlob->GetBufferPointer(), computeShaderBlob->GetBufferSize());
	initializeComputePipelineState_.Finalize();
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

	//リソースの状態を遷移
	commandContext.TransitionResource(*particleSystem->GetInstancingResource(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	//RootSignatureを設定
	commandContext.SetComputeRootSignature(initializeComputeRootSignature);

	//PipelineStateを設定
	commandContext.SetPipelineState(initializeComputePipelineState_);

	//UAVを設定
	commandContext.SetComputeDescriptorTable(0, particleSystem->GetInstancingResource()->GetUAVHandle());

	//Dispatch
	commandContext.Dispatch(ParticleSystem::kMaxInstance, 1, 1);

	//リソースの状態を遷移
	commandContext.TransitionResource(*particleSystem->GetInstancingResource(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

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
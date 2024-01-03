#include "PostProcess.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Utilities/ShaderCompiler.h"

PostProcess* PostProcess::instance_ = nullptr;

PostProcess* PostProcess::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new PostProcess();
	}
	return instance_;
}

void PostProcess::Destroy()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void PostProcess::Initialize()
{
	CreateColorBuffers();

	CreateVertexBuffer();

	CreateMultiPassPipelineState();

	CreateGaussianBlurPipelineState();

	CreatePostProcessPipelineState();

	//GaussianBlurの作成
	for (uint32_t i = 0; i < 2; ++i)
	{
		gaussianBlurs_[i] = std::make_unique<GaussianBlur>();
		gaussianBlurs_[i]->Initialize(Application::kClientWidth / (i + 1), Application::kClientHeight / (i + 1));
	}

	//Bloomの作成
	bloom_ = std::make_unique<Bloom>();
	bloom_->Initialize();

	//Fogの作成
	fog_ = std::make_unique<Fog>();
	fog_->Initialize();

	//DepthofFieldの作成
	depthofField_ = std::make_unique<DepthofField>();
	depthofField_->Initialize();

	//LensDistortionの作成
	lensDistortion_ = std::make_unique<LensDistortion>();
	lensDistortion_->Initialize();

	//Vignetteの作成
	vignette_ = std::make_unique<Vignette>();
	vignette_->Initialize();
}

void PostProcess::Update()
{
	//Bloomの更新
	bloom_->Update();

	//Fogの更新
	fog_->Update();

	//DepthofFieldの更新
	depthofField_->Update();

	//LensDistortionの更新
	lensDistortion_->Update();

	//Vignetteの更新
	vignette_->Update();
}

void PostProcess::Draw()
{
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();
	TextureManager* textureManager = TextureManager::GetInstance();
	Renderer* renderer = Renderer::GetInstance();
	commandContext->SetRootSignature(postProcessRootSignature_);
	commandContext->SetPipelineState(postProcessPipelineState_);
	commandContext->SetVertexBuffer(vertexBufferView_);
	commandContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandContext->SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, textureManager->GetDescriptorHeap()->GetDescriptorHeap());
	commandContext->SetDescriptorTable(0, renderer->GetSceneDepthDescriptorHandle());
	commandContext->SetDescriptorTable(1, gaussianBlurs_[0]->GetDescriptorHandle(GaussianBlur::kVertical));
	commandContext->SetDescriptorTable(2, gaussianBlurs_[0]->GetHighLumDescriptorHandle(GaussianBlur::kVertical));
	commandContext->SetDescriptorTable(3, gaussianBlurs_[1]->GetDescriptorHandle(GaussianBlur::kVertical));
	commandContext->SetDescriptorTable(4, gaussianBlurs_[1]->GetHighLumDescriptorHandle(GaussianBlur::kVertical));
	commandContext->SetConstantBuffer(5, bloom_->GetConstBuffer()->GetGpuVirtualAddress());
	commandContext->SetConstantBuffer(6, fog_->GetConstBuffer()->GetGpuVirtualAddress());
	commandContext->SetConstantBuffer(7, depthofField_->GetConstBuffer()->GetGpuVirtualAddress());
	commandContext->SetConstantBuffer(8, lensDistortion_->GetConstBuffer()->GetGpuVirtualAddress());
	commandContext->SetConstantBuffer(9, vignette_->GetConstBuffer()->GetGpuVirtualAddress());
	commandContext->DrawInstanced(6, 1);
}

void PostProcess::ApplyPostProcess()
{
	//二パス目の描画
	SecondPassDraw();

	if (!isEnable_)
	{
		return;
	}

	//横ぼかし
	ApplyGaussianBlur(kNormal, GaussianBlur::kHorizontal);

	//縦ぼかし
	ApplyGaussianBlur(kNormal, GaussianBlur::kVertical);

	//縮小横ぼかし
	ApplyGaussianBlur(kDownSample, GaussianBlur::kHorizontal);

	//縮小縦ぼかし
	ApplyGaussianBlur(kDownSample, GaussianBlur::kVertical);
}

void PostProcess::CreateColorBuffers()
{
	//マルチパス用のリソースの作成
	multiPassColorBuffer_ = std::make_unique<ColorBuffer>();
	multiPassColorBuffer_->Create(Application::kClientWidth, Application::kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

	//ディスクリプターをコピー
	ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();
	multiPassDescriptorHandle_ = TextureManager::GetInstance()->GetDescriptorHeap()->Allocate();
	device->CopyDescriptorsSimple(1, multiPassDescriptorHandle_, multiPassColorBuffer_->GetSRVHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//高輝度用のリソースの作成
	highLumColorBuffer_ = std::make_unique<ColorBuffer>();
	highLumColorBuffer_->Create(Application::kClientWidth, Application::kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

	//ディスクリプターをコピー
	highLumDescriptorHandle_ = TextureManager::GetInstance()->GetDescriptorHeap()->Allocate();
	device->CopyDescriptorsSimple(1, highLumDescriptorHandle_, highLumColorBuffer_->GetSRVHandle(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void PostProcess::CreateVertexBuffer()
{
	//頂点の作成
	vertices_[0] = VertexDataPosUV{ {-1.0f,-1.0f,1.0,1.0f},{0.0f,1.0f} };
	vertices_[1] = VertexDataPosUV{ {-1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f} };
	vertices_[2] = VertexDataPosUV{ {1.0f,-1.0f,1.0f,1.0f},{1.0f,1.0f} };
	vertices_[3] = VertexDataPosUV{ {-1.0f,1.0f,1.0f,1.0f},{0.0f,0.0f} };
	vertices_[4] = VertexDataPosUV{ {1.0f,1.0f,1.0f,1.0f},{1.0f,0.0f} };
	vertices_[5] = VertexDataPosUV{ {1.0f,-1.0f,1.0f,1.0f},{1.0f,1.0f} };

	//頂点リソースを作る
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sizeof(VertexDataPosUV) * 6);
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexDataPosUV) * 6;
	//頂点1つあたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUV);

	//頂点バッファにデータを書き込む
	VertexDataPosUV* vertexData = static_cast<VertexDataPosUV*>(vertexBuffer_->Map());
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexDataPosUV) * vertices_.size());
	vertexBuffer_->Unmap();
}

void PostProcess::CreateMultiPassPipelineState()
{
	//RootSignatureの作成
	multiPassRootSignature_.Create(1, 1);

	//RootParameterの設定
	multiPassRootSignature_[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);

	//StaticSamplerを設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	multiPassRootSignature_.InitStaticSampler(0, staticSamplers[0], D3D12_SHADER_VISIBILITY_PIXEL);
	multiPassRootSignature_.Finalize();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//シェーダーをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = ShaderCompiler::CompileShader(L"MultiPassVS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = ShaderCompiler::CompileShader(L"MultiPassPS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//書き込むRTVの情報
	DXGI_FORMAT rtvFormats[2];
	rtvFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//PSOを作成する
	multiPassPipelineState_.SetRootSignature(&multiPassRootSignature_);
	multiPassPipelineState_.SetInputLayout(2, inputElementDescs);
	multiPassPipelineState_.SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
	multiPassPipelineState_.SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
	multiPassPipelineState_.SetBlendState(blendDesc);
	multiPassPipelineState_.SetRasterizerState(rasterizerDesc);
	multiPassPipelineState_.SetRenderTargetFormats(2, rtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);
	multiPassPipelineState_.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	multiPassPipelineState_.SetSampleMask(D3D12_DEFAULT_SAMPLE_MASK);
	multiPassPipelineState_.Finalize();
}

void PostProcess::CreateGaussianBlurPipelineState()
{
	//RootSignatureの作成
	gaussianBlurRootSignature_.Create(3, 1);

	//RootParameterの設定
	gaussianBlurRootSignature_[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	gaussianBlurRootSignature_[1].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	gaussianBlurRootSignature_[2].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);

	//StaticSamplerを設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	gaussianBlurRootSignature_.InitStaticSampler(0, staticSamplers[0], D3D12_SHADER_VISIBILITY_PIXEL);
	gaussianBlurRootSignature_.Finalize();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//シェーダーをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob[2];
	vertexShaderBlob[0] = ShaderCompiler::CompileShader(L"HorizontalBlurVS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob[0] != nullptr);
	vertexShaderBlob[1] = ShaderCompiler::CompileShader(L"VerticalBlurVS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob[1] != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob[2];
	pixelShaderBlob [0] = ShaderCompiler::CompileShader(L"HorizontalBlurPS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob[0] != nullptr);
	pixelShaderBlob[1] = ShaderCompiler::CompileShader(L"VerticalBlurPS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob[1] != nullptr);

	//書き込むRTVの情報
	DXGI_FORMAT rtvFormats[2];
	rtvFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//PSOを作成する
	for (uint32_t i = 0; i < gaussianBlurPipelineStates_.size(); ++i)
	{
		gaussianBlurPipelineStates_[i].SetRootSignature(&gaussianBlurRootSignature_);
		gaussianBlurPipelineStates_[i].SetInputLayout(2, inputElementDescs);
		gaussianBlurPipelineStates_[i].SetVertexShader(vertexShaderBlob[i]->GetBufferPointer(), vertexShaderBlob[i]->GetBufferSize());
		gaussianBlurPipelineStates_[i].SetPixelShader(pixelShaderBlob[i]->GetBufferPointer(), pixelShaderBlob[i]->GetBufferSize());
		gaussianBlurPipelineStates_[i].SetBlendState(blendDesc);
		gaussianBlurPipelineStates_[i].SetRasterizerState(rasterizerDesc);
		gaussianBlurPipelineStates_[i].SetRenderTargetFormats(2, rtvFormats, DXGI_FORMAT_D24_UNORM_S8_UINT);
		gaussianBlurPipelineStates_[i].SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		gaussianBlurPipelineStates_[i].SetSampleMask(D3D12_DEFAULT_SAMPLE_MASK);
		gaussianBlurPipelineStates_[i].Finalize();
	}
}

void PostProcess::CreatePostProcessPipelineState()
{
	//RootSignatureの作成
	postProcessRootSignature_.Create(10, 1);

	//RootParameterの設定
	postProcessRootSignature_[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 3, D3D12_SHADER_VISIBILITY_PIXEL);
	postProcessRootSignature_[1].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	postProcessRootSignature_[2].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	postProcessRootSignature_[3].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 5, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	postProcessRootSignature_[4].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 6, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	postProcessRootSignature_[5].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	postProcessRootSignature_[6].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_PIXEL);
	postProcessRootSignature_[7].InitAsConstantBuffer(2, D3D12_SHADER_VISIBILITY_PIXEL);
	postProcessRootSignature_[8].InitAsConstantBuffer(3, D3D12_SHADER_VISIBILITY_PIXEL);
	postProcessRootSignature_[9].InitAsConstantBuffer(4, D3D12_SHADER_VISIBILITY_PIXEL);

	//StaticSamplerを設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	postProcessRootSignature_.InitStaticSampler(0, staticSamplers[0], D3D12_SHADER_VISIBILITY_PIXEL);
	postProcessRootSignature_.Finalize();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//シェーダーをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = ShaderCompiler::CompileShader(L"PostEffectsVS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = ShaderCompiler::CompileShader(L"PostEffectsPS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//書き込むRTVの情報
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//PSOを作成する
	postProcessPipelineState_.SetRootSignature(&postProcessRootSignature_);
	postProcessPipelineState_.SetInputLayout(2, inputElementDescs);
	postProcessPipelineState_.SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
	postProcessPipelineState_.SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
	postProcessPipelineState_.SetBlendState(blendDesc);
	postProcessPipelineState_.SetRasterizerState(rasterizerDesc);
	postProcessPipelineState_.SetRenderTargetFormats(1, &rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);
	postProcessPipelineState_.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	postProcessPipelineState_.SetSampleMask(D3D12_DEFAULT_SAMPLE_MASK);
	postProcessPipelineState_.SetDepthStencilState(depthStencilDesc);
	postProcessPipelineState_.Finalize();
}

void PostProcess::SecondPassDraw()
{
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();
	TextureManager* textureManager = TextureManager::GetInstance();
	Renderer* renderer = Renderer::GetInstance();

	//Resourceの状態遷移
	commandContext->TransitionResource(*multiPassColorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandContext->TransitionResource(*highLumColorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//RTVの設定
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	rtvHandles[0] = multiPassColorBuffer_->GetRTVHandle();
	rtvHandles[1] = highLumColorBuffer_->GetRTVHandle();
	commandContext->SetRenderTargets(2, rtvHandles);

	//RenderTargetをクリア
	commandContext->ClearColor(*multiPassColorBuffer_);
	commandContext->ClearColor(*highLumColorBuffer_);

	//RootSignatureを設定
	commandContext->SetRootSignature(multiPassRootSignature_);

	//PipelineStateを設定
	commandContext->SetPipelineState(multiPassPipelineState_);

	//VertexBufferViewを設定
	commandContext->SetVertexBuffer(vertexBufferView_);

	//形状を設定
	commandContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//DescriptorHeapを設定
	commandContext->SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, textureManager->GetDescriptorHeap()->GetDescriptorHeap());

	//DescriptorTableを設定
	commandContext->SetDescriptorTable(0, renderer->GetSceneColorDescriptorHandle());

	//DrawCall
	commandContext->DrawInstanced(6, 1);

	//Resourceの状態遷移
	commandContext->TransitionResource(*multiPassColorBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandContext->TransitionResource(*highLumColorBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void PostProcess::ApplyGaussianBlur(BlurType type,GaussianBlur::BlurDirection direction)
{
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();
	TextureManager* textureManager = TextureManager::GetInstance();

	//描画前処理
	gaussianBlurs_[type]->PreBlur(direction);

	//RootSignatureを設定
	commandContext->SetRootSignature(gaussianBlurRootSignature_);

	//PipelineStateを設定
	commandContext->SetPipelineState(gaussianBlurPipelineStates_[direction]);

	//VertexBufferViewを設定
	commandContext->SetVertexBuffer(vertexBufferView_);

	//形状を設定
	commandContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//DescriptorHeapを設定
	commandContext->SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, textureManager->GetDescriptorHeap()->GetDescriptorHeap());

	//DescriptorTableを設定
	if (type == kNormal && direction == GaussianBlur::kHorizontal)
	{
		commandContext->SetDescriptorTable(0, multiPassDescriptorHandle_);
		commandContext->SetDescriptorTable(1, highLumDescriptorHandle_);
	}
	else if (type == kDownSample && direction == GaussianBlur::kHorizontal)
	{
		commandContext->SetDescriptorTable(0, gaussianBlurs_[kNormal]->GetDescriptorHandle(GaussianBlur::kVertical));
		commandContext->SetDescriptorTable(1, gaussianBlurs_[kNormal]->GetHighLumDescriptorHandle(GaussianBlur::kVertical));
	}
	else
	{
		commandContext->SetDescriptorTable(0, gaussianBlurs_[type]->GetDescriptorHandle(GaussianBlur::kHorizontal));
		commandContext->SetDescriptorTable(1, gaussianBlurs_[type]->GetHighLumDescriptorHandle(GaussianBlur::kHorizontal));

	}

	//CBVを設定
	commandContext->SetConstantBuffer(2, gaussianBlurs_[type]->GetConstantBuffer()->GetGpuVirtualAddress());

	//DrawCall
	commandContext->DrawInstanced(6, 1);

	//描画後処理
	gaussianBlurs_[type]->PostBlur(direction);
}
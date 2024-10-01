#include "TrailRenderer.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Utilities/ShaderCompiler.h"

//実体定義
TrailRenderer* TrailRenderer::instance_ = nullptr;

TrailRenderer* TrailRenderer::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new TrailRenderer();
	}
	return instance_;
}

void TrailRenderer::Destroy()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void TrailRenderer::Initialize()
{
	//頂点バッファの作成
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sizeof(VertexDataPosUV) * kMaxVertices);

	//頂点バッファビューの作成
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
	vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUV);
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexDataPosUV) * kMaxVertices);

	//テクスチャを設定
	SetTexture("white.png");

	//RootSignatureの作成
	rootSignature_.Create(2, 1);
	rootSignature_[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
	rootSignature_[1].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);

	//StaticSamplerを設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
	staticSamplers[0].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;//比較結果をバイリニア補間
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//0~1の範囲外をクランプ
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;//<=であればtrue(1.0f),そうでなければ(0.0f)
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSamplers[0].MaxAnisotropy = 1;//深度傾斜を有効に
	rootSignature_.InitStaticSampler(0, staticSamplers[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignature_.Finalize();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2]{};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

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
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = ShaderCompiler::CompileShader(L"Trail.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = ShaderCompiler::CompileShader(L"Trail.PS.hlsl", L"ps_6_0");
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

	//PSOの作成
	pipelineState_.SetRootSignature(&rootSignature_);
	pipelineState_.SetInputLayout(2, inputElementDescs);
	pipelineState_.SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
	pipelineState_.SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
	pipelineState_.SetBlendState(blendDesc);
	pipelineState_.SetRasterizerState(rasterizerDesc);
	pipelineState_.SetRenderTargetFormats(1, &rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);
	pipelineState_.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pipelineState_.SetSampleMask(D3D12_DEFAULT_SAMPLE_MASK);
	pipelineState_.SetDepthStencilState(depthStencilDesc);
	pipelineState_.Finalize();
}

void TrailRenderer::Update()
{

}

void TrailRenderer::Draw()
{
	if (!vertices_.empty())
	{
		//カメラが無ければ止める
		assert(camera_);

		// コマンドリストを取得
		CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();

		//RootSignatureを設定
		commandContext->SetRootSignature(rootSignature_);

		//PipelineStateを設定
		commandContext->SetPipelineState(pipelineState_);

		//VertexBufferViewを設定
		commandContext->SetVertexBuffer(vertexBufferView_);

		//形状を設定。PSOに設定しているものとは別。同じものを設定すると考えておけば良い
		commandContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

		//Cameraを設定
		commandContext->SetConstantBuffer(0, camera_->GetConstantBuffer()->GetGpuVirtualAddress());

		//Textureを設定
		commandContext->SetDescriptorTable(1, texture_->GetSRVHandle());

		//描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
		commandContext->DrawInstanced((UINT)vertices_.size(), 1);
	}
}

void TrailRenderer::SetTexture(const std::string& textureName)
{
	//テクスチャを設定
	texture_ = TextureManager::GetInstance()->FindTexture(textureName);
	//テクスチャがなかったら止める
	assert(texture_);
}
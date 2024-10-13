#include "TrailRenderer.h"
#include "Engine/Base/GraphicsCore.h"
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

Trail* TrailRenderer::CreateTrail(const std::string& name)
{
	Trail* trail = TrailRenderer::GetInstance()->CreateTrailInternal(name);
	return trail;
}

void TrailRenderer::Initialize()
{
	//RootSignatureの作成
	rootSignature_.Create(3, 1);
	rootSignature_[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
	rootSignature_[1].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignature_[2].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);

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
	//全ての軌跡の更新
	for (const auto& trail : trails_)
	{
		trail.second->Update();
	}
}

void TrailRenderer::Draw()
{
	//軌跡が無ければ処理を飛ばす
	if (trails_.empty()) return;

	//カメラが無ければ止める
	assert(camera_);

	//コマンドリストを取得
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();

	//RootSignatureを設定
	commandContext->SetRootSignature(rootSignature_);

	//PipelineStateを設定
	commandContext->SetPipelineState(pipelineState_);

	//形状を設定。PSOに設定しているものとは別。同じものを設定すると考えておけば良い
	commandContext->SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//Cameraを設定
	commandContext->SetConstantBuffer(0, camera_->GetConstantBuffer()->GetGpuVirtualAddress());

	//全ての軌跡を描画
	for (const auto& trail : trails_)
	{
		//頂点数を取得
		UINT numVertices = (UINT)trail.second->GetNumVertices();

		//頂点が無かったら飛ばす
		if (numVertices == 0)
		{
			continue;
		}

		//VertexBufferViewを設定
		commandContext->SetVertexBuffer(trail.second->GetVertexBufferView());

		//Materialを設定
		commandContext->SetConstantBuffer(1, trail.second->GetMaterialResource()->GetGpuVirtualAddress());

		//Textureを設定
		commandContext->SetDescriptorTable(2, trail.second->GetTexture()->GetSRVHandle());

		//描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
		commandContext->DrawInstanced(numVertices, 1);
	}
}

Trail* TrailRenderer::CreateTrailInternal(const std::string& name)
{
	auto it = trails_.find(name);
	if (it != trails_.end())
	{
		return it->second.get();
	}

	Trail* newTrail = new Trail();
	newTrail->Initialize();
	trails_[name] = std::unique_ptr<Trail>(newTrail);
	return newTrail;
}
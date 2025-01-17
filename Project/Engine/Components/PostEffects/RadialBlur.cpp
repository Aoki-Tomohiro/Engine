/**
 * @file RadialBlur.cpp
 * @brief ラジアルブラーを管理するクラス
 * @author 青木智滉
 * @date
 */

#include "RadialBlur.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Utilities/ShaderCompiler.h"

void RadialBlur::Initialize()
{
	//ColorBufferの生成
	colorBuffer_ = std::make_unique<ColorBuffer>();
	colorBuffer_->Create(Application::kClientWidth, Application::kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

	//ConstBufferの生成
	constBuff_ = std::make_unique<UploadBuffer>();
	constBuff_->Create(sizeof(ConstBuffDataRadialBlur));
	Update();

	//PipelineStateの生成
	CreatePipelineState();
}

void RadialBlur::Update()
{
	ConstBuffDataRadialBlur* radialBlurData = static_cast<ConstBuffDataRadialBlur*>(constBuff_->Map());
	radialBlurData->center = center_;
	radialBlurData->blurWidth = blurWidth_;
	constBuff_->Unmap();
}

void RadialBlur::Apply(const DescriptorHandle& srvHandle)
{
	if (!isEnable_)
	{
		return;
	}

	//コマンドリストを取得
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();

	//リソースの状態遷移
	commandContext->TransitionResource(*colorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//RenderTargetを設定
	commandContext->SetRenderTargets(1, &colorBuffer_->GetRTVHandle());

	//RenderTargetをクリア
	commandContext->ClearColor(*colorBuffer_);

	//RootSignatureを設定
	commandContext->SetRootSignature(rootSignature_);

	//PipelineStateを設定
	commandContext->SetPipelineState(pipelineState_);

	//DescriptorTableを設定
	commandContext->SetDescriptorTable(0, srvHandle);

	//CBVを設定
	commandContext->SetConstantBuffer(1, constBuff_->GetGpuVirtualAddress());

	//ビューポート
	D3D12_VIEWPORT viewport{ 0.0f, 0.0f, Application::kClientWidth, Application::kClientHeight, 0.0f, 1.0f };
	commandContext->SetViewport(viewport);

	//シザー矩形を設定
	D3D12_RECT scissorRect{ 0, 0, Application::kClientWidth, Application::kClientHeight };
	commandContext->SetScissor(scissorRect);

	//DrawCall
	commandContext->DrawInstanced(6, 1);

	//リソースの状態遷移
	commandContext->TransitionResource(*colorBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void RadialBlur::CreatePipelineState()
{
	//RootSignatureの作成
	rootSignature_.Create(2, 1);

	//RootParameterの設定
	rootSignature_[0].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignature_[1].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);

	//StaticSamplerを設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;//0~1の範囲外をクランプ
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	rootSignature_.InitStaticSampler(0, staticSamplers[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignature_.Finalize();

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
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = nullptr;
	vertexShaderBlob = ShaderCompiler::CompileShader(L"RadialBlur.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = nullptr;
	pixelShaderBlob = ShaderCompiler::CompileShader(L"RadialBlur.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//書き込むRTVの情報
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//PSOを作成する
	pipelineState_.SetRootSignature(&rootSignature_);
	pipelineState_.SetInputLayout(2, inputElementDescs);
	pipelineState_.SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
	pipelineState_.SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
	pipelineState_.SetBlendState(blendDesc);
	pipelineState_.SetRasterizerState(rasterizerDesc);
	pipelineState_.SetRenderTargetFormats(1, &rtvFormat, DXGI_FORMAT_D32_FLOAT);
	pipelineState_.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pipelineState_.SetSampleMask(D3D12_DEFAULT_SAMPLE_MASK);
	pipelineState_.Finalize();
}
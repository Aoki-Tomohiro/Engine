#include "LineRenderer.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Utilities/ShaderCompiler.h"

//実体定義
LineRenderer* LineRenderer::instance_ = nullptr;

LineRenderer* LineRenderer::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new LineRenderer();
	}
	return instance_;
}

void LineRenderer::Destroy()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void LineRenderer::Initialize()
{
	//頂点バッファの作成
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sizeof(Vector4) * kMaxLines * 2);

	//VertexBufferViewの作成
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
	vertexBufferView_.StrideInBytes = sizeof(Vector4);
	vertexBufferView_.SizeInBytes = UINT(sizeof(Vector4) * kMaxLines * 2);

	//RootSignatureの作成
	rootSignature_.Create(1, 0);
	rootSignature_[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
	rootSignature_.Finalize();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

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
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = ShaderCompiler::CompileShader(L"Line.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = ShaderCompiler::CompileShader(L"Line.PS.hlsl", L"ps_6_0");
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
	pipelineState_.SetInputLayout(1, inputElementDescs);
	pipelineState_.SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
	pipelineState_.SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
	pipelineState_.SetBlendState(blendDesc);
	pipelineState_.SetRasterizerState(rasterizerDesc);
	pipelineState_.SetRenderTargetFormats(1, &rtvFormat, DXGI_FORMAT_D24_UNORM_S8_UINT);
	pipelineState_.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
	pipelineState_.SetSampleMask(D3D12_DEFAULT_SAMPLE_MASK);
	pipelineState_.SetDepthStencilState(depthStencilDesc);
	pipelineState_.Finalize();
}

void LineRenderer::AddLine(const Vector3& start, const Vector3& end)
{
	//頂点データを追加
	vertices_.push_back(Vector4({ start.x,start.y,start.z,1.0f }));
	vertices_.push_back(Vector4({ end.x,end.y,end.z,1.0f }));

	//頂点データを書き込む
	Vector4* vertexData = static_cast<Vector4*>(vertexBuffer_->Map());
	std::memcpy(vertexData, vertices_.data(), sizeof(Vector4) * vertices_.size());
	vertexBuffer_->Unmap();
}

void LineRenderer::Draw()
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

		//描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
		commandContext->DrawInstanced((UINT)vertices_.size(), 1);
	}

	//頂点をクリア
	vertices_.clear();
}
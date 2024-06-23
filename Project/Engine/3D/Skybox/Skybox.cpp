#include "Skybox.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/ShaderCompiler.h"

//実体定義
Skybox* Skybox::instance_ = nullptr;

Skybox* Skybox::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new Skybox();
	}
	return instance_;
}

void Skybox::Destroy()
{
	if (instance_ != nullptr)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void Skybox::Initialize()
{
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 500.0f,500.0f,500.0f };

	//テクスチャを設定
	TextureManager::Load("Skybox.dds");
	SetTexture("Skybox.dds");

	//PipelineStateの作成
	CreatePipelineState();

	//頂点バッファの作成
	CreateVertexBuffer();

	//IndexBufferの作成
	CreateIndexBuffer();

	//マテリアル用のリソースの作成
	CreateMaterialResource();
}

void Skybox::Update()
{
	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();

	//マテリアルの更新
	UpdateMaterialResource();
}

void Skybox::Draw()
{
	//カメラがない場合は描画しない
	if (!camera_)
	{
		return;
	}

	//CommandContextを取得
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();

	//TextureManagerを取得
	TextureManager* textureManager = TextureManager::GetInstance();

	//RootSignatureを設定
	commandContext->SetRootSignature(rootSignature_);

	//PipelineStateを設定
	commandContext->SetPipelineState(pipelineState_);

	//VertexBufferを設定
	commandContext->SetVertexBuffer(vertexBufferView_);

	//形状を設定
	commandContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//IndexBufferを設定
	commandContext->SetIndexBuffer(indexBufferView_);

	//Materialを設定
	commandContext->SetConstantBuffer(0, materialConstBuffer_->GetGpuVirtualAddress());

	//WorldTransformを設定
	commandContext->SetConstantBuffer(1, worldTransform_.GetConstantBuffer()->GetGpuVirtualAddress());

	//Cameraを設定
	commandContext->SetConstantBuffer(2, camera_->GetConstantBuffer()->GetGpuVirtualAddress());

	//Textureを設定
	commandContext->SetDescriptorTable(3, texture_->GetSRVHandle());

	//描画
	commandContext->DrawIndexedInstanced(kMaxIndices, 1);
}

void Skybox::CreatePipelineState()
{
	rootSignature_.Create(4, 1);
	rootSignature_[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignature_[1].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
	rootSignature_[2].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_VERTEX);
	rootSignature_[3].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 1, D3D12_SHADER_VISIBILITY_PIXEL);

	//StaticSamplerを設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1]{};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	rootSignature_.InitStaticSampler(0, staticSamplers[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootSignature_.Finalize();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1]{};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//ブレンドなし
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = ShaderCompiler::CompileShader(L"Skybox.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = ShaderCompiler::CompileShader(L"Skybox.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//比較はするのでDepth自体は有効
	depthStencilDesc.DepthEnable = true;
	//全ピクセルがz = 1に出力されるので、わざわざ書き込む必要がない
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//今までと同様に比較
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//書き込むRTVの情報
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	pipelineState_.SetRootSignature(&rootSignature_);
	pipelineState_.SetInputLayout(1, inputElementDescs);
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

void Skybox::CreateVertexBuffer()
{
	//右面。描画インデックスは[0,1,2][2,1,3]で内側を向く
	vertices_[0].position = { 1.0f,1.0f,1.0f,1.0f };
	vertices_[1].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertices_[2].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertices_[3].position = { 1.0f,-1.0f,-1.0f,1.0f };
	//左面。描画インデックスは[4,5,6][6,5,7]
	vertices_[4].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertices_[5].position = { -1.0f,1.0f,1.0f,1.0f };
	vertices_[6].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertices_[7].position = { -1.0f,-1.0f,1.0f,1.0f };
	//前面。描画インデックスは[8,9,10][10,9,11]
	vertices_[8].position = { -1.0f,1.0f,1.0f,1.0f };
	vertices_[9].position = { 1.0f,1.0f,1.0f,1.0f };
	vertices_[10].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertices_[11].position = { 1.0f,-1.0f,1.0f,1.0f };
	//後面。描画インデックスは[12,13,14][14,13,15]
	vertices_[12].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertices_[13].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertices_[14].position = { 1.0f,-1.0f,-1.0f,1.0f };
	vertices_[15].position = { -1.0f,-1.0f,-1.0f,1.0f };
	//上面。描画インデックスは[16,17,18][18,17,19]
	vertices_[16].position = { -1.0f,1.0f,-1.0f,1.0f };
	vertices_[17].position = { 1.0f,1.0f,-1.0f,1.0f };
	vertices_[18].position = { -1.0f,1.0f,1.0f,1.0f };
	vertices_[19].position = { 1.0f,1.0f,1.0f,1.0f };
	//下面。描画インデックスは[20,21,22][22,21,23]
	vertices_[20].position = { -1.0f,-1.0f,1.0f,1.0f };
	vertices_[21].position = { 1.0f,-1.0f,1.0f,1.0f };
	vertices_[22].position = { -1.0f,-1.0f,-1.0f,1.0f };
	vertices_[23].position = { 1.0f,-1.0f,-1.0f,1.0f };

	//頂点リソースを作る
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sizeof(VertexDataPosUV) * kMaxVertices);
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexDataPosUV) * kMaxVertices;
	//頂点1つあたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUV);

	//頂点バッファにデータを書き込む
	VertexDataPosUV* vertexData = static_cast<VertexDataPosUV*>(vertexBuffer_->Map());
	std::memcpy(vertexData, vertices_.data(), sizeof(VertexDataPosUV) * vertices_.size());
	vertexBuffer_->Unmap();
}

void Skybox::CreateIndexBuffer()
{
	//右面。描画インデックスは[0,1,2][2,1,3]で内側を向く
	indices_[0] = 0;
	indices_[1] = 1;
	indices_[2] = 2;
	indices_[3] = 2;
	indices_[4] = 1;
	indices_[5] = 3;
	//左面。描画インデックスは[4,5,6][6,5,7]
	indices_[6] = 4;
	indices_[7] = 5;
	indices_[8] = 6;
	indices_[9] = 6;
	indices_[10] = 5;
	indices_[11] = 7;
	//前面。描画インデックスは[8,9,10][10,9,11]
	indices_[12] = 8;
	indices_[13] = 9;
	indices_[14] = 10;
	indices_[15] = 10;
	indices_[16] = 9;
	indices_[17] = 11;
	//後面。描画インデックスは[12,13,14][14,13,15]
	indices_[18] = 12;
	indices_[19] = 13;
	indices_[20] = 14;
	indices_[21] = 14;
	indices_[22] = 13;
	indices_[23] = 15;
	//上面。描画インデックスは[16,17,18][18,17,19]
	indices_[24] = 16;
	indices_[25] = 17;
	indices_[26] = 18;
	indices_[27] = 18;
	indices_[28] = 17;
	indices_[29] = 19;
	//下面。描画インデックスは[20,21,22][22,21,23]
	indices_[30] = 20;
	indices_[31] = 21;
	indices_[32] = 22;
	indices_[33] = 22;
	indices_[34] = 21;
	indices_[35] = 23;

	//インデックスバッファの作成
	indexBuffer_ = std::make_unique<UploadBuffer>();
	indexBuffer_->Create(sizeof(uint32_t) * indices_.size());

	//インデックスバッファビューの作成
	indexBufferView_.BufferLocation = indexBuffer_->GetGpuVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * indices_.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスバッファのデータを書き込む
	uint32_t* indexData = static_cast<uint32_t*>(indexBuffer_->Map());
	std::memcpy(indexData, indices_.data(), sizeof(uint32_t) * indices_.size());
	indexBuffer_->Unmap();
}

void Skybox::CreateMaterialResource()
{
	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialConstBuffer_ = std::make_unique<UploadBuffer>();
	materialConstBuffer_->Create(sizeof(ConstBuffDataMaterial));

	//マテリアル用のリソースの更新
	UpdateMaterialResource();
}

void Skybox::UpdateMaterialResource()
{
	//書き込むためのアドレスを取得
	ConstBuffDataMaterial* materialData = static_cast<ConstBuffDataMaterial*>(materialConstBuffer_->Map());
	//データを書き込む
	materialData->color = color_;
	materialData->uvTransform = Mathf::MakeIdentity4x4();
	materialConstBuffer_->Unmap();
}

void Skybox::SetTexture(const std::string& textureName)
{
	//テクスチャを設定
	texture_ = TextureManager::GetInstance()->FindTexture(textureName);
	//テクスチャがなかったら止める
	assert(texture_);
}
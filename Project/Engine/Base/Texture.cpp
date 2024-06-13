#include "Texture.h"
#include "GraphicsCore.h"

void Texture::Create(const DirectX::ScratchImage& mipImages)
{
	ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();

	currentState_ = D3D12_RESOURCE_STATE_COPY_DEST;

	//メタデータを取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//metadataを基にResourceの設定
	resourceDesc_.Width = UINT(metadata.width);//Textureの幅
	resourceDesc_.Height = UINT(metadata.height);//Textureの高さ
	resourceDesc_.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc_.DepthOrArraySize = UINT16(metadata.arraySize);//奥行or配列Textureの配列数
	resourceDesc_.Format = metadata.format;//TextureのFormat
	resourceDesc_.SampleDesc.Count = 1;//サンプルカウント。1固定
	resourceDesc_.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段使っているのは2次元

	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc_, currentState_, nullptr,
		IID_PPV_ARGS(&resource_));
	assert(SUCCEEDED(hr));

	//SRVの作成
	CreateDerivedViews(device, metadata);

	//テクスチャのリソースにデータを転送する
	UploadTextureData(resource_.Get(), mipImages);
}

void Texture::CreateDerivedViews(ID3D12Device* device, const DirectX::TexMetadata& metadata)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//metadataからcubemapかどうか取得
	if (metadata.IsCubemap())
	{
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;//unionがTextureCubeになったが、内部パラメータの意味はTexture2dと変わらない
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}
	else
	{
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	}
	srvHandle_ = GraphicsCore::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);
}

void Texture::UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages)
{
	ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();
	GraphicsContext graphicsContext{};
	graphicsContext.Initialize();
	CommandQueue commandQueue{};
	commandQueue.Initialize();

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));

	//リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;//UploadHeapを使う

	//リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = intermediateSize;
	//バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//作成
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&intermediateResource));
	assert(SUCCEEDED(hr));

	UpdateSubresources(graphicsContext.GetCommandList(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

	graphicsContext.TransitionResource(*this, D3D12_RESOURCE_STATE_GENERIC_READ);
	graphicsContext.Close();
	ID3D12CommandList* commandLists[] = { graphicsContext.GetCommandList() };
	commandQueue.ExecuteCommandList(commandLists);
	commandQueue.WaitForFence();
	graphicsContext.Reset();
}
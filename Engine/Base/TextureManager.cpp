#include "TextureManager.h"
#include "GraphicsCore.h"
#include "Engine/Utilities/Log.h"

//実体定義
TextureManager* TextureManager::instance_ = nullptr;

TextureManager* TextureManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new TextureManager();
	}
	return instance_;
}

void TextureManager::Destroy()
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void TextureManager::Load(const std::string& filePath)
{
	TextureManager::GetInstance()->LoadInternal(filePath);
}

void TextureManager::Initialize()
{
	srvDescriptorHeap_ = std::make_unique<DescriptorHeap>();
	srvDescriptorHeap_->Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kNumDescriptors_, true);
	LoadInternal("Project/Resources/Images/white.png");
}

D3D12_RESOURCE_DESC TextureManager::GetResourceDesc(const std::string& name)
{
	auto it = textures_.find(name);
	if (it != textures_.end())
	{
		return it->second->texture.GetResourceDesc();
	}
	return D3D12_RESOURCE_DESC();
}

DescriptorHandle TextureManager::GetDescriptorHandle(const std::string& name)
{
	auto it = textures_.find(name);
	if (it != textures_.end())
	{
		DescriptorHandle descriptorHandle = it->second->descriptorHandle;
		return descriptorHandle;
	}
	return DescriptorHandle();
}

void TextureManager::LoadInternal(const std::string& filePath)
{
	auto it = textures_.find(filePath);
	if (it != textures_.end())
	{
		return;
	}

	//テクスチャの作成
	std::unique_ptr<TextureResource> texture = std::make_unique<TextureResource>();
	texture->texture.Create(filePath);

	//Descriptorをコピー
	ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();
	texture->descriptorHandle = srvDescriptorHeap_->Allocate();
	device->CopyDescriptorsSimple(1, texture->descriptorHandle, texture->texture.GetSRV(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//コンテナに追加
	textures_[filePath] = std::move(texture);
}
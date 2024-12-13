/**
 * @file TextureManager.cpp
 * @brief テクスチャの読み込みや管理をするファイル
 * @author 青木智滉
 * @date
 */

#include "TextureManager.h"
#include "Engine/Utilities/Log.h"

//実体定義
TextureManager* TextureManager::instance_ = nullptr;
const std::string TextureManager::kBaseDirectory = "Application/Resources/Images";

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
	LoadInternal("white.png");
}

const Texture* TextureManager::FindTexture(const std::string& name) const
{
	//テクスチャを探す
	if (textures_.contains(name))
	{
		return textures_.at(name).get();
	}

	return nullptr;
}

void TextureManager::LoadInternal(const std::string& filename)
{
	auto it = textures_.find(filename);
	if (it != textures_.end())
	{
		return;
	}

	//テクスチャを読み込む
	std::string filePath = kBaseDirectory + "/" + filename;
	if (filename.find("Application/Resources/Models") != std::string::npos)
	{
		filePath = filename;
	}
	else if (filename.find("Application/Resources/Images") != std::string::npos)
	{
		filePath = filename;
	}
	DirectX::ScratchImage mipImages = LoadTexture(filePath);

	//テクスチャの作成
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	texture->Create(mipImages);

	//コンテナに追加
	textures_[filename] = std::move(texture);
}

DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath) {
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = MyUtility::ConvertString(filePath);
	HRESULT hr;
	//.ddsで終わっていたらddsとみなす。より安全な方法はいくらでもあるので余裕があれば対応すると良い
	if (filePathW.ends_with(L".dds"))
	{
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		assert(SUCCEEDED(hr));
	}
	else
	{
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hr));
	}

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	//圧縮フォーマットか調べる
	if (DirectX::IsCompressed(image.GetMetadata().format))
	{
		//圧縮フォーマットならそのまま使うのでmoveする
		mipImages = std::move(image);
	}
	else
	{
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
		assert(SUCCEEDED(hr));
	}

	//ミップマップ付きのデータを返す
	return mipImages;
}
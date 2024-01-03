#pragma once
#include "Texture.h"
#include "DescriptorHeap.h"

class TextureManager
{
public:
	struct TextureResource
	{
		Texture texture{};
		DescriptorHandle descriptorHandle{};
	};

	static const int kNumDescriptors_ = 256;

	static TextureManager* GetInstance();

	static void Destroy();

	static void Load(const std::string& filePath);

	void Initialize();

	DescriptorHandle GetDescriptorHandle(const std::string& name);

	D3D12_RESOURCE_DESC GetResourceDesc(const std::string& name);

	DescriptorHeap* GetDescriptorHeap() const { return srvDescriptorHeap_.get(); };

private:
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	void LoadInternal(const std::string& filePath);

private:
	static TextureManager* instance_;

	std::unordered_map<std::string, std::unique_ptr<TextureResource>> textures_{};

	std::unique_ptr<DescriptorHeap> srvDescriptorHeap_ = nullptr;
};


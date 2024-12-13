/**
 * @file TextureManager.h
 * @brief テクスチャの読み込みや管理をするファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Texture.h"

class TextureManager
{
public:
	//ディレクトリパス
	static const std::string kBaseDirectory;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static TextureManager* GetInstance();

	/// <summary>
	/// 破棄処理
	/// </summary>
	static void Destroy();

	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="filename">ファイルの名前</param>
	static void Load(const std::string& filename);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// テクスチャを探す
	/// </summary>
	/// <param name="name">テクスチャの名前</param>
	/// <returns>テクスチャ</returns>
	const Texture* FindTexture(const std::string& name) const;

private:
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	/// <summary>
	/// テクスチャを内部で読み込む
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	void LoadInternal(const std::string& filePath);

	/// <summary>
	/// テクスチャを読み込む
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>スクラッチイメージ</returns>
	DirectX::ScratchImage LoadTexture(const std::string& filePath);

private:
	static TextureManager* instance_;

	std::unordered_map<std::string, std::unique_ptr<Texture>> textures_{};
};


/**
 * @file AnimationManager.h
 * @brief アニメーションデータの読み込みや管理をするファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Animation.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class AnimationManager
{
public:
	//ディレクトリパス
	static const std::string kBaseDirectory;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static AnimationManager* GetInstance();

	/// <summary>
	/// 破棄処理
	/// </summary>
	static void Destroy();

	/// <summary>
	/// アニメーションを作成
	/// </summary>
	/// <param name="fileName">ファイルの名前</param>
	/// <returns>アニメーション</returns>
	static Animation* Create(const std::string& fileName);

private:
	AnimationManager() = default;
	~AnimationManager() = default;
	AnimationManager(const AnimationManager&) = delete;
	AnimationManager& operator=(const AnimationManager&) = delete;

	/// <summary>
	/// アニメーションを内部で作成
	/// </summary>
	/// <param name="fileName">ファイルの名前</param>
	/// <returns>アニメーション</returns>
	Animation* CreateInternal(const std::string& fileName);

	/// <summary>
	/// アニメーションファイルの読み込み
	/// </summary>
	/// <param name="directoryPath">ディレクトリパス</param>
	/// <param name="filename">ファイルの名前</param>
	/// <returns>読み込んだアニメーションデータ</returns>
	std::vector<Animation::AnimationData> LoadAnimationFile(const std::string& directoryPath, const std::string& filename);

private:
	static AnimationManager* instance_;

	std::map<std::string, std::vector<Animation::AnimationData>> animationDatas_{};
};


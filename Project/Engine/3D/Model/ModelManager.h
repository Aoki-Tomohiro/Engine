/**
 * @file ModelManager.h
 * @brief モデルの読み込みと管理をするファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <filesystem>

class ModelManager
{
public:
	//ディレクトリパス
	static const std::string kBaseDirectory;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns></returns>
	static ModelManager* GetInstance();

	/// <summary>
	/// 破棄処理
	/// </summary>
	static void Destroy();

	/// <summary>
	/// モデルファイルからモデルを作成
	/// </summary>
	/// <param name="modelName">モデルの名前</param>
	/// <param name="drawPass">描画の種類</param>
	/// <returns>モデル</returns>
	static Model* CreateFromModelFile(const std::string& modelName, DrawPass drawPass);

private:
	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(const ModelManager&) = delete;
	ModelManager& operator=(const ModelManager&) = delete;

	/// <summary>
	/// モデルを内部で作成
	/// </summary>
	/// <param name="modelName">モデルの名前</param>
	/// <param name="drawPass">描画の種類</param>
	/// <returns>モデル</returns>
	Model* CreateInternal(const std::string& modelName, DrawPass drawPass);

	/// <summary>
	/// 再利用できるモデルを探す
	/// </summary>
	/// <param name="modelName">モデルの名前</param>
	/// <returns>モデル</returns>
	Model* FindReusableModel(const std::string& modelName);

	/// <summary>
	/// モデルデータからモデルを生成
	/// </summary>
	/// <param name="modelData">モデルデータ</param>
	/// <param name="modelName">モデルの名前</param>
	/// <param name="drawPass">描画の種類</param>
	/// <returns>モデル</returns>
	Model* CreateModelFromData(const Model::ModelData& modelData, const std::string& modelName, DrawPass drawPass);

	/// <summary>
	/// モデルファイルを探す
	/// </summary>
	/// <param name="modelName">モデルの名前</param>
	/// <returns>ファイル名</returns>
	std::string FindModelFile(const std::string& modelName);

	/// <summary>
	/// モデルファイルの読み込み
	/// </summary>
	/// <param name="directoryPath">ディレクトリパス</param>
	/// <param name="filename">ファイルの名前</param>
	/// <returns>モデルデータ</returns>
	Model::ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	Model::Node ReadNode(aiNode* node);

private:
	static ModelManager* instance_;

	std::map<std::string, Model::ModelData> modelDatas_{};

	std::map<std::string, std::vector<std::unique_ptr<Model>>> models_{};
};


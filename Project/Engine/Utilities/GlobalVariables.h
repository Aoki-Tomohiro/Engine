/**
 * @file GlobalVariables.h
 * @brief 環境変数を編集、読み込み、出力する関数
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/Vector3.h"
#include <iostream>
#include <fstream>
#include <variant>
#include <string>
#include <map>
#include <Engine/Externals/nlohmann/json.hpp>

class GlobalVariables
{
public:
	using json = nlohmann::json;
	using Item = std::variant<int32_t, float, Vector3>;
	using Group = std::map<std::string, Item>;

	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static GlobalVariables* GetInstance();

	/// <summary>
	/// 新しいグループの作成
	/// </summary>
	/// <param name="groupName">グループの名前</param>
	void CreateGroup(const std::string& groupName);

	/// <summary>
	/// ファイルに出力
	/// </summary>
	/// <param name="groupName">出力するデータのグループ名</param>
	void SaveFile(const std::string& groupName);

	/// <summary>
	/// すべてのファイルを読み込む
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// ファイルを読み込む
	/// </summary>
	/// <param name="groupName">読み込むグループ名</param>
	void LoadFile(const std::string& groupName);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	//値のセット(int)
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	//値のセット(float)
	void SetValue(const std::string& groupName, const std::string& key, float value);
	//値のセット(Vector3)
	void SetValue(const std::string& groupName, const std::string& key, const Vector3& value);

	//値の取得(int)
	int32_t GetIntValue(const std::string& groupName, const std::string& key) const;
	//値の取得(float)
	float GetFloatValue(const std::string& groupName, const std::string& key) const;
	//値の取得(Vector3)
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key) const;

	//項目の追加(int)
	void AddItem(const std::string& groupName, const std::string& key, int32_t value);
	//項目の追加(float)
	void AddItem(const std::string& groupName, const std::string& key, float value);
	//項目の追加(Vector3)
	void AddItem(const std::string& groupName, const std::string& key, const Vector3& value);

public:
	const std::string kDirectoryPath = "Application/Resources/Config/";

private:
	GlobalVariables() = default;
	~GlobalVariables() = default;
	GlobalVariables(const GlobalVariables&) = delete;
	const GlobalVariables& operator=(const GlobalVariables&) = delete;

private:
	// 全データ
	std::map<std::string, Group> datas_;
};


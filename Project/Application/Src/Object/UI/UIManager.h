/**
 * @file UIManager.h
 * @brief 全てのUIを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "InteractiveUI.h"
#include "DynamicUI.h"
#include <fstream>
#include <map>

class UIManager
{
public:
	//CSVの列インデックスを列挙型で定義
	enum class CSVColumns 
	{
		Name,
		Type,
		TextureName,
		PositionX,
		PositionY,
		ScaleX,
		ScaleY,
		AnchorX,
		AnchorY,
		ParameterStart,
	};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	void Initialize(const std::string& fileName);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// UIの追加
	/// </summary>
	/// <param name="uiName">UIの名前</param>
	/// <param name="uiElement">UI</param>
	void AddUI(const std::string& uiName, std::unique_ptr<UIElement> uiElement);

	/// <summary>
	/// UIを取得
	/// </summary>
	/// <typeparam name="Type">UIのタイプ</typeparam>
	/// <param name="uiName">UIの名前</param>
	/// <returns>UI</returns>
	template<typename Type>
	Type* GetUI(const std::string& uiName);

private:
	/// <summary>
	/// 通常のUIを追加
	/// </summary>
	/// <param name="name">UIの名前</param>
	/// <param name="textureName">テクスチャの名前</param>
	/// <param name="position">座標</param>
	/// <param name="scale">スケール</param>
	/// <param name="anchorPoint">アンカーポイント</param>
	void AddStaticUI(const std::string& name, const std::string& textureName, const Vector2& position, const Vector2& scale, const Vector2& anchorPoint);

	/// <summary>
	/// インタラクティブなUIを追加
	/// </summary>
	/// <param name="name">UIの名前</param>
	/// <param name="textureName">テクスチャの名前</param>
	/// <param name="position">座標</param>
	/// <param name="scale">スケール</param>
	/// <param name="tokens">要素の配列</param>
	/// <param name="anchorPoint">アンカーポイント</param>
	void AddInteractiveUI(const std::string& name, const std::string& textureName, const Vector2& position, const Vector2& scale, const std::vector<std::string>& tokens, const Vector2& anchorPoint);

	/// <summary>
	/// ダイナミックなUIを追加
	/// </summary>
	/// <param name="name">UIの名前</param>
	/// <param name="textureName">テクスチャの名前</param>
	/// <param name="position">座標</param>
	/// <param name="scale">スケール</param>
	/// <param name="tokens">要素の配列</param>
	/// <param name="anchorPoint">アンカーポイント</param>
	void AddDynamicUI(const std::string& name, const std::string& textureName, const Vector2& position, const Vector2& scale, const std::vector<std::string>& tokens, const Vector2& anchorPoint);

	/// <summary>
	/// ボタンのタイプを列挙体に変換して返す関数
	/// </summary>
	/// <param name="buttonTypeStr">ボタンタイプの文字列</param>
	/// <returns>ボタンのタイプ</returns>
	InteractiveUI::ButtonType ParseButtonType(const std::string& buttonTypeStr);

	/// <summary>
	/// CSVの1行をカンマ区切りで分割して文字列のベクターとして返す関数
	/// </summary>
	/// <param name="line">CSVの1行の文字列</param>
	/// <returns>分割された文字列のベクター</returns>
	std::vector<std::string> SplitCSVLine(const std::string& line);

private:
	//UIのマップ
	std::map<std::string, std::unique_ptr<UIElement>> uiElements_{};

	//追加順を保持するリスト
	std::vector<std::string> uiOrder_{};
};


template<typename Type>
inline Type* UIManager::GetUI(const std::string& uiName)
{
	//名前でUI要素を検索
	auto it = uiElements_.find(uiName);

	//指定した名前のUIが見つかった場合
	if (it != uiElements_.end())
	{
		//型が一致する場合は返す
		Type* uiElement = dynamic_cast<Type*>(it->second.get());
		if (uiElement) 
		{
			return uiElement;
		}
	}
	return nullptr;  //型が一致しない場合
}
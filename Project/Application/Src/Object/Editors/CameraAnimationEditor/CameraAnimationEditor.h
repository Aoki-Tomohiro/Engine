#pragma once
#include "Engine/Base/ImGuiManager.h"
#include "CameraPath.h"
#include <Engine/Externals/nlohmann/json.hpp>
#include <fstream>
#include <map>

/// <summary>
/// カメラアニメーションを管理するクラス
/// </summary>
class CameraAnimationEditor
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	//カメラパスを取得
	CameraPath& GetCameraPath(const std::string& name) { return cameraPaths_[name]; };

	//全てのカメラパスを取得
	const std::map<std::string, CameraPath>& GetCameraPaths() const { return cameraPaths_; };

	//現在のキーフレームを取得
	const CameraPath::CameraKeyFrame& GetNewKeyFrame() const { return newKeyFrame_; };

	//デバッグのフラグを取得
	const bool GetIsDebug() const { return isDebug_; };

	//現在のアニメーションを再生するかどうかのフラグを取得
	const bool GetIsPlayingCurrentCameraAnimation() const { return isPlayingCurrentCameraAnimation_; };

	//現在編集中のカメラパスの名前を取得
	const std::string& GetCurrentEditPathName() const { return currentEditPathName_; };

private:
	//保存先ファイルパス
	const std::string kDirectoryPath = "Application/Resources/Config/CameraAnimations/";

	/// <summary>
	/// カメラアニメーションを追加
	/// </summary>
	void AddCameraAnimation();

	/// <summary>
	/// デバッグ用のコントロールを表示
	/// </summary>
	void DisplayDebugControls();

	/// <summary>
	/// カメラパスのコントロールを表示
	/// </summary>
	/// <param name="cameraPathName">カメラパスの名前</param>
	void DisplayCameraPathControls(const std::string& cameraPathName);

	/// <summary>
	/// ファイルに保存
	/// </summary>
	/// <param name="cameraPathName">カメラパスの名前</param>
	void SaveFile(const std::string& cameraPathName);

	/// <summary>
	/// 全てのファイルの読み込み
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// ファイルに保存
	/// </summary>
	/// <param name="cameraPathName">カメラパスの名前</param>
	void LoadFile(const std::string& cameraPathName);

	/// <summary>
	/// キーフレームを追加
	/// </summary>
	/// <param name="cameraPath">カメラパス</param>
	void AddKeyFrame(CameraPath& cameraPath);

	/// <summary>
	/// キーフレームを編集
	/// </summary>
	/// <param name="cameraPath">カメラパス</param>
	void EditKeyFrames(CameraPath& cameraPath);

	/// <summary>
	/// コンテナの中から要素を選択するためのComboBoxを作成する関数
	/// </summary>
	/// <typeparam name="Type">コンテナに格納されている値の型</typeparam>
	/// <param name="label">ラベル名</param>
	/// <param name="selectedName">選択された要素の名前</param>
	/// <param name="items">表示する名前とその設定が格納されたコンテナ</param>
	template<typename Type>
	void SelectFromMap(const char* label, std::string& selectedName, const std::map<std::string, Type>& items);

private:
	//カメラパス
	std::map<std::string, CameraPath> cameraPaths_{};

	//新しいカメラパス
	CameraPath::CameraKeyFrame newKeyFrame_{};

	//デバッグ中かどうか
	bool isDebug_ = false;

	//現在編集中のカメラパスの名前
	std::string currentEditPathName_{};

	//現在編集中のカメラアニメーションを再生したかどうか
	bool isPlayingCurrentCameraAnimation_ = false;
};


template<typename Type>
inline void CameraAnimationEditor::SelectFromMap(const char* label, std::string& selectedName, const std::map<std::string, Type>& items)
{
	//ラベルを表示
	ImGui::Text(label);

	//ComboBoxの作成
	if (ImGui::BeginCombo(label, selectedName.c_str()))
	{
		//全てのアイテムのキーをリスト表示
		for (const auto& [name, setting] : items)
		{
			//選択されたかどうかを判別
			const bool isSelected = (selectedName == name);

			//選択されていた場合、名前を更新
			if (ImGui::Selectable(name.c_str(), isSelected))
			{
				selectedName = name;
			}

			//選択されていた場合はデフォルトフォーカスを設定
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		//ComboBoxの終了
		ImGui::EndCombo();
	}
};
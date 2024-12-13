/**
 * @file CameraAnimationEditor.cpp
 * @brief カメラアニメーションを編集・管理するファイル
 * @author 青木智滉
 * @date
 */

#include "CameraAnimationEditor.h"

namespace
{
	static const char* EASING_TYPES[] = { "Linear", "EaseIn", "EaseOut", "EaseInOut" };
}

void CameraAnimationEditor::Initialize()
{
	//すべてのファイルを読み込む
	LoadFiles();
}

void CameraAnimationEditor::Update()
{
	//カメラパスを追加
	AddCameraAnimation();

	//カメラパスが存在しない場合は処理を飛ばす
	if (cameraPaths_.empty()) { return; };

	//編集するカメラパスを選択
	SelectFromMap("カメラアニメーション一覧", currentEditPathName_, cameraPaths_);

	//現在のカメラパスの設定を取得
	auto effectIt = cameraPaths_.find(currentEditPathName_);
	//カメラパスが見つからなければ処理を飛ばす
	if (effectIt == cameraPaths_.end()) return;

	//デバッグのコントロールを表示
	DisplayDebugControls();

	//選択したカメラパスのコントロールを表示
	DisplayCameraPathControls(currentEditPathName_);

	//アニメーション終了後の補間リセット設定
	ImGui::SeparatorText("アニメーション終了後の補間設定");
	EditInterpolationSettings(effectIt->second);

	//キーフレームを追加
	ImGui::SeparatorText("キーフレームを追加");
	AddKeyFrame(effectIt->second);

	//全てのキーフレームの編集
	ImGui::SeparatorText("キーフレームを編集");
	EditKeyFrames(effectIt->second);
}

void CameraAnimationEditor::AddCameraAnimation()
{
	//名前入力用のバッファ
	static char newCameraPathName[128] = { '\0' };

	//新しいカメラパスの名前を入力
	ImGui::InputText("新しいカメラアニメーションの名前", newCameraPathName, sizeof(newCameraPathName));

	//新しいカメラパスを追加
	if (ImGui::Button("カメラアニメーションを追加"))
	{
		//名前をstd::stringに格納
		std::string cameraPathName(newCameraPathName);

		//名前が入力されていないまたは同じ名前のカメラパスが存在していた場合は飛ばす
		if (cameraPathName.empty() || cameraPaths_.find(cameraPathName) != cameraPaths_.end())
		{
			return;
		}

		//新しいパーティクルエフェクトの設定を追加
		cameraPaths_[cameraPathName] = CameraPath();
		//入力バッファのクリア
		std::memset(newCameraPathName, '\0', sizeof(newCameraPathName));
	}
}

void CameraAnimationEditor::DisplayDebugControls()
{
	//デバッグのフラグの設定
	ImGui::Checkbox("デバッグ状態にするかどうか", &isDebug_);

	//デバッグのフラグが立っているとき
	if (isDebug_)
	{
		if (ImGui::Button("アニメーションを再生"))
		{
			isPlayingCurrentCameraAnimation_ = true;
		}
		else
		{
			isPlayingCurrentCameraAnimation_ = false;
		}
	}
}

void CameraAnimationEditor::DisplayCameraPathControls(const std::string& cameraPathName)
{
	//保存
	if (ImGui::Button("保存"))
	{
		SaveFile(cameraPathName);
	}

	//次のアイテムを同じ行に配置
	ImGui::SameLine();

	//読み込み
	if (ImGui::Button("読み込み"))
	{
		LoadFile(cameraPathName);
	}
}

void CameraAnimationEditor::EditInterpolationSettings(CameraPath& cameraPath)
{
	//現在選択されているイージングタイプのインデックスを取得
	int selectedIndex = static_cast<int>(cameraPath.GetResetEasingType());

	//列挙体から選択するためのGUI
	if (ImGui::Combo("イージングの種類", &selectedIndex, EASING_TYPES, IM_ARRAYSIZE(EASING_TYPES)))
	{
		cameraPath.SetResetEasingType(static_cast<CameraPath::EasingType>(selectedIndex));
	}

	//補間速度リセットが完了するまでの時間を設定
	float resetInterpolationSpeedGraduallyTime = cameraPath.GetResetDuration();
	ImGui::DragFloat("リセット時間", &resetInterpolationSpeedGraduallyTime, 0.01f);
	cameraPath.SetResetDuration(resetInterpolationSpeedGraduallyTime);
}

void CameraAnimationEditor::SaveFile(const std::string& cameraPathName)
{
	//カメラパスを取得
	CameraPath& cameraPath = cameraPaths_[cameraPathName];

	//ルートの JSON オブジェクトを作成
	nlohmann::json root = nlohmann::json::object();

	//カメラパスの JSON オブジェクトを作成
	nlohmann::json cameraPathJson = nlohmann::json::object();

	//キーフレームの配列を作成
	nlohmann::json keyFramesJson = nlohmann::json::array();

	//全てのキーフレームを保存する
	for (int32_t i = 0; i < cameraPath.GetKeyFrameCount(); ++i)
	{
		//キーフレームの JSON オブジェクトを作成
		nlohmann::json keyFrameJson = nlohmann::json::object();

		// ーフレームを取得
		const CameraPath::CameraKeyFrame& keyFrame = cameraPath.GetCameraKeyFrame(i);

		//キーフレームの情報を書き込む
		keyFrameJson["Time"] = keyFrame.time;
		keyFrameJson["Position"] = { keyFrame.position.x, keyFrame.position.y, keyFrame.position.z };
		keyFrameJson["Rotation"] = { keyFrame.rotation.x, keyFrame.rotation.y, keyFrame.rotation.z, keyFrame.rotation.w };
		keyFrameJson["Fov"] = keyFrame.fov;
		keyFrameJson["EasingType"] = EasingTypeToString(keyFrame.easingType); // EasingType の文字列に変換

		//キーフレームを配列に追加
		keyFramesJson.push_back(keyFrameJson);
	}

	//キーフレーム配列を追加
	cameraPathJson["KeyFrames"] = keyFramesJson; 
	//補間速度リセットのイージングタイプ
	cameraPathJson["ResetEasingType"] = EasingTypeToString(cameraPath.GetResetEasingType()); 
	//補間リセット時間
	cameraPathJson["ResetDuration"] = cameraPath.GetResetDuration(); 

	//ルートにカメラパスを追加
	root[cameraPathName] = cameraPathJson;

	//ディレクトリがなければ作成する
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(kDirectoryPath))
	{
		std::filesystem::create_directory(kDirectoryPath);
	}

	//書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + cameraPathName + ".json";
	//書き込み用ファイルストリーム
	std::ofstream ofs;
	//ファイルを書き込み用に開く
	ofs.open(filePath);
	//ファイルオープン失敗
	if (ofs.fail())
	{
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "CameraAnimationEditor", 0);
		assert(0);
		return;
	}

	//ファイルにjson文字列を書き込む
	ofs << std::setw(4) << root << std::endl;
	//ファイルを閉じる
	ofs.close();
}

std::string CameraAnimationEditor::EasingTypeToString(CameraPath::EasingType easingType) const
{
	//イージングタイプを文字列に変換
	switch (easingType)
	{
	case CameraPath::EasingType::kEaseIn: return "EaseIn";
	case CameraPath::EasingType::kEaseOut: return "EaseOut";
	case CameraPath::EasingType::kEaseInOut: return "EaseInOut";
	case CameraPath::EasingType::kLinear: return "Linear";
	default: return "Unknown";
	}
}

void CameraAnimationEditor::LoadFiles()
{
	//保存先のディレクトリパス
	std::filesystem::path dir(kDirectoryPath);
	//ディレクトリが無ければスキップする
	if (!std::filesystem::exists(kDirectoryPath))
	{
		return;
	}

	//ディレクトリを検索
	std::filesystem::directory_iterator dir_it(kDirectoryPath);
	for (const std::filesystem::directory_entry& entry : dir_it)
	{
		//ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		//ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		//.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0)
		{
			continue;
		}

		//ファイルを読み込む
		LoadFile(filePath.stem().string());
	}
}

void CameraAnimationEditor::LoadFile(const std::string& cameraPathName)
{
	//読み込むjsonファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + cameraPathName + ".json";
	//読み込み用のファイルストリーム
	std::ifstream ifs;
	//ファイルを読み込みように開く
	ifs.open(filePath);
	//ファイルオープン失敗
	if (ifs.fail())
	{
		std::string message = "Failed open data file for read.";
		MessageBoxA(nullptr, message.c_str(), "CameraAnimationEditor", 0);
		assert(0);
		return;
	}

	//json文字列からjsonのデータ構造に展開
	nlohmann::json root;
	ifs >> root;
	//ファイルを閉じる
	ifs.close();

	//jsonに何も書かれていなければ処理を飛ばす
	if (root.empty()) { return; };

	//カメラパスのデータを取得
	nlohmann::json cameraPathJson = root[cameraPathName];

	//キーフレーム配列を取得
	nlohmann::json keyFramesJson = cameraPathJson["KeyFrames"];

	//カメラパスの設定を取得
	CameraPath& cameraPath = cameraPaths_[cameraPathName];
	cameraPath.ClearKeyFrames();

	//全てのキーフレームを取得
	for (nlohmann::json::iterator keyFrameItem = keyFramesJson.begin(); keyFrameItem != keyFramesJson.end(); ++keyFrameItem)
	{
		//キーフレームのデータを読み込む
		nlohmann::json keyFrameJson = keyFrameItem.value();
		//追加するキーフレーム
		CameraPath::CameraKeyFrame cameraKeyFrame{};
		cameraKeyFrame.time = keyFrameJson["Time"];
		cameraKeyFrame.position = { keyFrameJson["Position"][0].get<float>(), keyFrameJson["Position"][1].get<float>(), keyFrameJson["Position"][2].get<float>() };
		cameraKeyFrame.rotation = { keyFrameJson["Rotation"][0].get<float>(), keyFrameJson["Rotation"][1].get<float>(), keyFrameJson["Rotation"][2].get<float>(), keyFrameJson["Rotation"][3].get<float>() };
		cameraKeyFrame.fov = keyFrameJson["Fov"];
		//配列の各要素を走査
		for (int32_t i = 0; i < IM_ARRAYSIZE(EASING_TYPES); ++i)
		{
			//JSONの値が配列内の文字列と一致する場合
			if (keyFrameJson["EasingType"] == EASING_TYPES[i])
			{
				//一致したインデックスを列挙型の値として設定
				cameraKeyFrame.easingType = static_cast<CameraPath::EasingType>(i);
				break;
			}
		}
		//キーフレームを追加
		cameraPath.AddKeyFrame(cameraKeyFrame);
	}

	//補間速度リセット時のイージングタイプを取得
	for (int32_t i = 0; i < IM_ARRAYSIZE(EASING_TYPES); ++i)
	{
		if (cameraPathJson["ResetEasingType"] == EASING_TYPES[i])
		{
			cameraPath.SetResetEasingType(static_cast<CameraPath::EasingType>(i));
			break;
		}
	}

	//補間速度リセットが完了するまでの時間を取得
	float resetDuration = cameraPathJson["ResetDuration"];
	cameraPath.SetResetDuration(resetDuration);
}

void CameraAnimationEditor::AddKeyFrame(CameraPath& cameraPath)
{
	//時間を編集
	ImGui::DragFloat("時間", &newKeyFrame_.time, 0.01f);

	//座標を編集
	ImGui::DragFloat3("座標", &newKeyFrame_.position.x, 0.01f);

	//回転を編集
	ImGui::DragFloat4("回転", &newKeyFrame_.rotation.x, 0.01f);

	//回転を正規化
	newKeyFrame_.rotation = Mathf::Normalize(newKeyFrame_.rotation);

	//視野角
	ImGui::DragFloat("視野角", &newKeyFrame_.fov, 0.01f);

	//選択中の列挙体のインデックス
	int selectedIndex = static_cast<int>(newKeyFrame_.easingType);
	//列挙体を選択
	if (ImGui::Combo("イージングの種類", &selectedIndex, EASING_TYPES, IM_ARRAYSIZE(EASING_TYPES)))
	{
		newKeyFrame_.easingType = static_cast<CameraPath::EasingType>(selectedIndex);
	}

	//キーフレームを追加して設定を初期化
	if (ImGui::Button("キーフレームを追加"))
	{
		cameraPath.AddKeyFrame(newKeyFrame_);
		newKeyFrame_ = CameraPath::CameraKeyFrame();
	}
}

void CameraAnimationEditor::EditKeyFrames(CameraPath& cameraPath)
{
	//キーフレームの削除を準備するためのリスト
	static std::vector<size_t> indicesToRemove;

	//全てのキーフレームを編集
	for (int32_t i = 0; i < cameraPath.GetKeyFrameCount(); ++i)
	{
		//キーフレームを取得
		CameraPath::CameraKeyFrame& keyFrame = cameraPath.GetCameraKeyFrame(i);

		//ツリーノードの名前を設定
		std::string nodeName = "キーフレーム " + std::to_string(i);

		//ツリーノードを展開
		if (ImGui::TreeNode(nodeName.c_str()))
		{
			//時間を編集
			ImGui::DragFloat("時間", &keyFrame.time, 0.001f);

			//座標を編集
			ImGui::DragFloat3("座標", &keyFrame.position.x, 0.001f);

			//回転を編集
			ImGui::DragFloat4("回転", &keyFrame.rotation.x, 0.001f);

			//回転を正規化
			keyFrame.rotation = Mathf::Normalize(keyFrame.rotation);

			//視野角を編集
			ImGui::DragFloat("視野角", &keyFrame.fov, 0.01f);

			//選択中の列挙体のインデックス
			int selectedIndex = static_cast<int>(keyFrame.easingType);
			//列挙体を選択
			if (ImGui::Combo("イージングの種類", &selectedIndex, EASING_TYPES, IM_ARRAYSIZE(EASING_TYPES)))
			{
				keyFrame.easingType = static_cast<CameraPath::EasingType>(selectedIndex);
			}

			//削除ボタンを追加
			if (ImGui::Button("削除"))
			{
				indicesToRemove.push_back(i);
			}

			//ツリーノードを閉じる
			ImGui::TreePop();
		}
	}

	//削除対象のインデックスを逆順で処理
	for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it)
	{
		cameraPath.RemoveKeyFrame(*it);
	}
	//インデックスリストをクリア
	indicesToRemove.clear();
}
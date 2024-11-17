#include "CameraAnimationEditor.h"

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
	SelectFromMap("Camera Paths", currentEditPathName_, cameraPaths_);

	//現在のカメラパスの設定を取得
	auto effectIt = cameraPaths_.find(currentEditPathName_);
	//カメラパスが見つからなければ処理を飛ばす
	if (effectIt == cameraPaths_.end()) return;

	//デバッグのコントロールを表示
	DisplayDebugControls();

	//選択したカメラパスのコントロールを表示
	DisplayCameraPathControls(currentEditPathName_);

	//キーフレームを追加
	ImGui::SeparatorText("Add Key Frame");
	AddKeyFrame(effectIt->second);

	//全てのキーフレームの編集
	ImGui::SeparatorText("Edit Key Frame");
	EditKeyFrames(effectIt->second);
}

void CameraAnimationEditor::AddCameraAnimation()
{
	//名前入力用のバッファ
	static char newCameraPathName[128] = { '\0' };

	//新しいカメラパスの名前を入力
	ImGui::InputText("New Camera Path Name", newCameraPathName, sizeof(newCameraPathName));

	//新しいカメラパスを追加
	if (ImGui::Button("Add Camera Path"))
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
	ImGui::Checkbox("Is Debug", &isDebug_);

	//デバッグのフラグが立っているとき
	if (isDebug_)
	{
		if (ImGui::Button("Play Animation"))
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
	if (ImGui::Button("Save"))
	{
		SaveFile(cameraPathName);
	}

	//次のアイテムを同じ行に配置
	ImGui::SameLine();

	//読み込み
	if (ImGui::Button("Load"))
	{
		LoadFile(cameraPathName);
	}
}

void CameraAnimationEditor::SaveFile(const std::string& cameraPathName)
{
	//カメラパスを取得
	CameraPath& cameraPath = cameraPaths_[cameraPathName];

	//rootのjsonオブジェクトを作成
	nlohmann::json root = nlohmann::json::object();

	//カメラパスのjsonオブジェクトを作成
	nlohmann::json cameraPathJson = nlohmann::json::object();

	//全てのキーフレームを保存する
	for (int32_t i = 0; i < cameraPath.GetKeyFrameCount(); ++i)
	{
		//キーフレームのjsonオブジェクトを作成
		nlohmann::json keyFrameJson = nlohmann::json::object();

		//キーフレームを取得
		const CameraKeyFrame& keyFrame = cameraPath.GetCameraKeyFrame(i);

		//キーフレームの情報を書き込む
		keyFrameJson["Time"] = keyFrame.time;
		keyFrameJson["Position"] = nlohmann::json::array({ keyFrame.position.x,keyFrame.position.y,keyFrame.position.z });
		keyFrameJson["Rotation"] = nlohmann::json::array({ keyFrame.rotation.x,keyFrame.rotation.y,keyFrame.rotation.z,keyFrame.rotation.w });
		keyFrameJson["Fov"] = keyFrame.fov;
		cameraPathJson["KeyFrame" + std::to_string(i)] = keyFrameJson;
	}

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

	//カメラパスの設定を取得
	CameraPath& cameraPath = cameraPaths_[cameraPathName];
	cameraPath.ClearKeyFrames();

	//全てのキーフレームを取得
	for (nlohmann::json::iterator keyFrameItem = cameraPathJson.begin(); keyFrameItem != cameraPathJson.end(); ++keyFrameItem)
	{
		//キーフレームのデータを読み込む
		nlohmann::json keyFrameJson = keyFrameItem.value();
		//追加するキーフレーム
		CameraKeyFrame cameraKeyFrame{};
		cameraKeyFrame.time = keyFrameJson["Time"];
		cameraKeyFrame.position = { keyFrameJson["Position"][0].get<float>(), keyFrameJson["Position"][1].get<float>(),keyFrameJson["Position"][2].get<float>() };
		cameraKeyFrame.rotation = { keyFrameJson["Rotation"][0].get<float>(), keyFrameJson["Rotation"][1].get<float>(),keyFrameJson["Rotation"][2].get<float>(),keyFrameJson["Rotation"][3].get<float>() };
		cameraKeyFrame.fov = keyFrameJson["Fov"];
		cameraPath.AddKeyFrame(cameraKeyFrame);
	}
}

void CameraAnimationEditor::AddKeyFrame(CameraPath& cameraPath)
{
	//キーフレームを編集
	ImGui::DragFloat("Time", &newKeyFrame_.time, 0.01f);
	ImGui::DragFloat3("Position", &newKeyFrame_.position.x, 0.01f);
	ImGui::DragFloat4("Rotation", &newKeyFrame_.rotation.x, 0.01f);
	ImGui::DragFloat("Fov", &newKeyFrame_.fov, 0.01f);
	newKeyFrame_.rotation = Mathf::Normalize(newKeyFrame_.rotation);

	//キーフレームを追加して設定を初期化
	if (ImGui::Button("AddKeyFrame"))
	{
		cameraPath.AddKeyFrame(newKeyFrame_);
		newKeyFrame_ = CameraKeyFrame();
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
		CameraKeyFrame& keyFrame = cameraPath.GetCameraKeyFrame(i);

		//ツリーノードの名前を設定
		std::string nodeName = "KeyFrame" + std::to_string(i);

		//ツリーノードを展開
		if (ImGui::TreeNode(nodeName.c_str()))
		{
			//キーフレームを編集
			ImGui::DragFloat("Time", &keyFrame.time, 0.001f);
			ImGui::DragFloat3("Position", &keyFrame.position.x, 0.001f);
			ImGui::DragFloat4("Rotation", &keyFrame.rotation.x, 0.001f);
			ImGui::DragFloat("Fov", &keyFrame.fov, 0.01f);

			//削除ボタンを追加
			if (ImGui::Button("Delete"))
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
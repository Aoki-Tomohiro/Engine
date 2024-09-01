#include "CameraPathManager.h"

void CameraPathManager::Initialize()
{
    LoadFiles();
}

void CameraPathManager::Update()
{
	ImGui::Begin("CameraPathManager", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                SaveFiles();
            }

            if (ImGui::MenuItem("Load"))
            {
                LoadFiles();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    //編集モードにするかどうか
    ImGui::Checkbox("IsEditModeEnabled", &isEditModeEnabled_);

    //カメラパスの追加
    static char newCameraPathName[128] = "";
    ImGui::InputText("NewCameraPathName", newCameraPathName, sizeof(newCameraPathName));
    if (ImGui::Button("AddCameraPath"))
    {
        //新しいパスの名前を初期化
        std::string cameraPath(newCameraPathName);

        //カメラパスが空ではない場合
        if (!cameraPath.empty())
        {
            //重複していない場合カメラパスを追加
            if (cameraPaths_.find(cameraPath) == cameraPaths_.end())
            {
                cameraPaths_[cameraPath] = CameraPath();
                std::memset(newCameraPathName, 0, sizeof(newCameraPathName));
            }
        }
    }

    //カメラパスの名前を選択するためのコンボボックス
    static int currentPathIndex = 0;
    if (ImGui::BeginCombo("CameraPaths",currentPathName.c_str()))
    {
        //インデックス
        int index = 0;
        //全てのカメラパスを探す
        for (auto& [name, path] : cameraPaths_)
        {
            //選択中のカメラパスだった場合項目を追加
            bool isSelected = (currentPathName == name);
            if (ImGui::Selectable(name.c_str(), isSelected))
            {
                currentPathIndex = index;
                currentPathName = name;
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
            //インデックスを増やす
            index++;
        }
        ImGui::EndCombo();
    }

    //選択されたカメラパスのキーフレームを編集
    if (!currentPathName.empty())
    {
        //選択中のカメラパスを取得
        CameraPath& selectedPath = cameraPaths_[currentPathName];

        //アニメーションの再生
        if (ImGui::Button("PlayAnimation"))
        {
            isPlayAnimation_ = true;
            animationTime_ = 0.0f;
        }
        ImGui::Separator();

        //キーフレームを追加
        ImGui::DragFloat("NewTime", &newKeyFrame_.time, 0.01f);
        ImGui::DragFloat3("NewPosition", &newKeyFrame_.position.x, 0.01f);
        ImGui::DragFloat4("NewRotation", &newKeyFrame_.rotation.x, 0.01f);
        ImGui::DragFloat("NewFov", &newKeyFrame_.fov, 0.01f);
        newKeyFrame_.rotation = Mathf::Normalize(newKeyFrame_.rotation);
        if (ImGui::Button("AddKeyFrame"))
        {
            selectedPath.AddKeyFrame(newKeyFrame_);
        }
        ImGui::Separator();

        //キーフレームの削除を準備するためのリスト
        static std::vector<size_t> indicesToRemove;

        //全てのキーフレームについて
        for (size_t i = 0; i < selectedPath.GetKeyFrameCount(); ++i)
        {
            //キーフレームを取得
            CameraKeyFrame& keyFrame = selectedPath.GetCameraKeyFrame(i);
            keyFrame.rotation = Mathf::Normalize(keyFrame.rotation);

            //キーフレームを編集
            ImGui::PushID(static_cast<int>(i));
            ImGui::Text("KeyFrame%zu", i);
            ImGui::DragFloat("Time", &keyFrame.time, 0.01f);
            ImGui::DragFloat3("Position", &keyFrame.position.x, 0.01f);
            ImGui::DragFloat4("Rotation", &keyFrame.rotation.x, 0.01f);
            ImGui::DragFloat("Fov", &keyFrame.fov, 0.01f);

            //削除するキーフレームを設定
            if (ImGui::Button(("RemoveKeyFrame")))
            {
                indicesToRemove.push_back(i);
            }

            ImGui::Separator();
            ImGui::PopID();
        }

        //削除対象のインデックスを逆順で処理
        for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it)
        {
            size_t index = *it;
            selectedPath.RemoveKeyFrame(index);
        }
        //インデックスリストをクリア
        indicesToRemove.clear(); 
    }

    //カメラパスの削除ボタン
    if (ImGui::Button("RemoveCameraPath"))
    {
        if (cameraPaths_.find(currentPathName) != cameraPaths_.end())
        {
            cameraPaths_.erase(currentPathName);
            currentPathName.clear();
        }
    }

	ImGui::End();
}

const CameraKeyFrame CameraPathManager::GetCurrentKeyFrame()
{
    if (isPlayAnimation_)
    {
        animationTime_ += GameTimer::GetDeltaTime();
        float duration = cameraPaths_[currentPathName].GetDuration();
        animationTime_ = std::clamp(animationTime_, 0.0f, duration);
        if (animationTime_ < duration)
        {
            return cameraPaths_[currentPathName].GetInterpolatedKeyFrame(animationTime_);
        }
        else
        {
            isPlayAnimation_ = false;
            animationTime_ = 0.0f;
            return cameraPaths_[currentPathName].GetInterpolatedKeyFrame(duration);
        }
    }
    return CameraKeyFrame();
}

void CameraPathManager::SaveFiles()
{
    nlohmann::json root;
    //nlohmann::json型のデータコンテナにグループの全データをまとめる
    root = nlohmann::json::object();

    //全てのカメラパスをjsonオブジェクトにまとめる
    for (auto& [name, path] : cameraPaths_)
    {
        nlohmann::json pathJson;
        //キーフレームの配列を作成
        nlohmann::json keyFramesArray = nlohmann::json::array();
        //全てのキーフレームについて
        for (size_t i = 0; i < path.GetKeyFrameCount(); ++i)
        {
            //キーフレームを取得
            const CameraKeyFrame& keyFrame = path.GetCameraKeyFrame(i);
            nlohmann::json keyFrameJson;
            keyFrameJson["time"] = keyFrame.time;
            keyFrameJson["position"] = { keyFrame.position.x, keyFrame.position.y, keyFrame.position.z };
            keyFrameJson["rotation"] = { keyFrame.rotation.x, keyFrame.rotation.y, keyFrame.rotation.z, keyFrame.rotation.w };
            keyFrameJson["fov"] = keyFrame.fov;
            keyFramesArray.push_back(keyFrameJson);
        }
        //カメラパスのjsonに追加
        pathJson["keyframes"] = keyFramesArray;
        //ルートにカメラパスを追加
        root[name] = pathJson;
    }

    //書き込み用ファイルストリーム
    std::ofstream ofs;
    //ファイルを書き込み用に開く
    ofs.open(kFileName);
    //ファイルにjson文字列を書き込む
    ofs << std::setw(4) << root << std::endl;
    //ファイルを閉じる
    ofs.close();
}

void CameraPathManager::LoadFiles()
{
    //読み込み用ファイルストリーム
    std::ifstream ifs;
    //ファイルを読み込み用に開く
    ifs.open(kFileName);

    nlohmann::json root;
    //json文字列からjsonのデータ構造に展開
    ifs >> root;
    //ファイルを閉じる
    ifs.close();

    //CameraPathをクリア
    cameraPaths_.clear();

    //JSONデータをCameraPathに変換
    for (const auto& [name, path] : root.items())
    {
        //カメラパス
        CameraPath cameraPath;
        
        //キーフレームを復元
        if (path.contains("keyframes"))
        {
            for (const auto& keyFrameJson : path["keyframes"])
            {
                CameraKeyFrame keyFrame;
                keyFrame.time = keyFrameJson["time"].get<float>();
                keyFrame.position = {
                    keyFrameJson["position"][0].get<float>(),
                    keyFrameJson["position"][1].get<float>(),
                    keyFrameJson["position"][2].get<float>(),
                };
                keyFrame.rotation = {
                    keyFrameJson["rotation"][0].get<float>(),
                    keyFrameJson["rotation"][1].get<float>(),
                    keyFrameJson["rotation"][2].get<float>(),
                    keyFrameJson["rotation"][3].get<float>(),
                };
                keyFrame.fov = keyFrameJson["fov"].get<float>();
                cameraPath.AddKeyFrame(keyFrame);
            }
        }

        //カメラパスを追加
        cameraPaths_[name] = cameraPath;
    }
}
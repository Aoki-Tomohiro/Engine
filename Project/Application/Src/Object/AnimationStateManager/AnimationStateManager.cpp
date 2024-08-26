#include "AnimationStateManager.h"

void AnimationStateManager::Initialize()
{
    LoadFiles();
}

void AnimationStateManager::Update()
{
    ImGui::Begin("AnimationPhaseEditor", nullptr, ImGuiWindowFlags_MenuBar);

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

    static int selectedStateIndex = -1;
    static int selectedPhaseIndex = -1;

    //状態の追加
    static char newStateName[128] = "";
    ImGui::InputText("NewAnimationStateName", newStateName, sizeof(newStateName));
    if (ImGui::Button("AddAnimationState"))
    {
        std::string newState(newStateName);
        if (!newState.empty())
        {
            //重複チェック
            bool stateExists = std::any_of(phaseDatas_.begin(), phaseDatas_.end(), [&newState](const AnimationState& state) {
                return state.name == newState;
                });

            if (!stateExists)
            {
                //新しい状態を追加
                phaseDatas_.push_back({ newState, {} });
                std::memset(newStateName, 0, sizeof(newStateName));
            }
        }
    }

    //状態の選択と表示
    for (size_t i = 0; i < phaseDatas_.size(); ++i)
    {
        AnimationState& state = phaseDatas_[i];

        //フェーズをアニメーション時間順にソート
        std::sort(state.phases.begin(), state.phases.end(), [](const Phase& a, const Phase& b) {
            return a.animationTime < b.animationTime;
            });

        if (ImGui::TreeNode(state.name.c_str()))
        {
            selectedStateIndex = static_cast<int>(i);

            //フェーズの追加
            static char newPhaseName[128] = "";
            ImGui::InputText("NewPhaseName", newPhaseName, sizeof(newPhaseName));
            if (ImGui::Button("AddPhase"))
            {
                std::string newPhase(newPhaseName);
                if (!newPhase.empty())
                {
                    //重複チェック
                    bool phaseExists = std::any_of(state.phases.begin(), state.phases.end(), [&newPhase](const Phase& phase) {
                        return phase.name == newPhase;
                        });

                    if (!phaseExists)
                    {
                        //新しいフェーズを追加
                        state.phases.push_back({ newPhase, 0.0f });
                        std::memset(newPhaseName, 0, sizeof(newPhaseName));
                    }
                }
            }

            //フェーズの表示と編集
            for (size_t j = 0; j < state.phases.size(); ++j)
            {
                Phase& phase = state.phases[j];
                if (ImGui::TreeNode(phase.name.c_str()))
                {
                    selectedPhaseIndex = static_cast<int>(j);
                    ImGui::DragFloat("AnimationTime", &phase.animationTime, 0.01f);
                    ImGui::TreePop();

                    // フェーズの削除ボタン
                    if (ImGui::Button("DeletePhase"))
                    {
                        state.phases.erase(state.phases.begin() + j);
                        //削除された分だけインデックスを調整
                        --j;
                    }
                }
            }

            //状態の削除ボタン
            if (ImGui::Button("DeleteState"))
            {
                phaseDatas_.erase(phaseDatas_.begin() + i);
                //削除された分だけインデックスを調整
                --i;
                //インデックスをリセット
                selectedStateIndex = -1;
                //インデックスをリセット
                selectedPhaseIndex = -1;
            }

            ImGui::TreePop();
        }
    }

    ImGui::End();
}

const AnimationState AnimationStateManager::GetAnimationState(const std::string& name) const
{
    for (size_t i = 0; i < phaseDatas_.size(); ++i)
    {
        if (phaseDatas_[i].name == name)
        {
            return phaseDatas_[i];
        }
    }
    return AnimationState();
}

//void AnimationPhaseManager::Update()
//{
//    if (ImGui::Begin("Phase Editor"))
//    {
//        // Phaseデータの表示・編集
//        for (auto it = phaseDatas_.begin(); it != phaseDatas_.end();)
//        {
//            auto& [phaseName, phase] = *it;
//            if (ImGui::CollapsingHeader(phaseName.c_str()))
//            {
//                // Phase内の項目の表示・編集
//                for (auto itemIt = phase.phase_.begin(); itemIt != phase.phase_.end();)
//                {
//                    auto& [key, value] = *itemIt;
//
//                    ImGui::PushID(&key); // ユニークIDを生成
//
//                    static char keyBuffer[128];
//                    // strncpy_sの使用
//                    strncpy_s(keyBuffer, key.c_str(), _countof(keyBuffer) - 1);
//                    keyBuffer[_countof(keyBuffer) - 1] = '\0'; // 確実にnull終端
//                    if (ImGui::InputText("Phase Name", keyBuffer, _countof(keyBuffer)))
//                    {
//                        std::string newKey = keyBuffer;
//                        if (newKey != key)
//                        {
//                            // キーが変更された場合、一旦削除して再追加
//                            float valueCopy = value;
//                            itemIt = phase.phase_.erase(itemIt); // 現在の要素を削除
//                            phase.phase_[newKey] = valueCopy; // 新しいキーで再追加
//                            continue; // itemItはeraseで次の要素に移動済み
//                        }
//                    }
//                    ++itemIt;
//
//                    ImGui::InputFloat("Value", &value);
//                    ImGui::PopID();
//                }
//
//                // 新しい項目を追加するためのUI
//                static char newKey[128] = "";
//                static float newValue = 0.0f;
//
//                ImGui::InputText("New Phase Name", newKey, _countof(newKey));
//                ImGui::InputFloat("New Value", &newValue);
//
//                if (ImGui::Button("Add Phase Item"))
//                {
//                    if (newKey[0] != '\0')
//                    {
//                        phase.phase_[newKey] = newValue;
//                        newKey[0] = '\0'; // フィールドをリセット
//                        newValue = 0.0f;
//                    }
//                }
//
//                if (ImGui::Button("Remove Last Phase Item"))
//                {
//                    if (!phase.phase_.empty())
//                    {
//                        phase.phase_.erase(std::prev(phase.phase_.end())->first);
//                    }
//                }
//            }
//            ++it;
//        }
//
//        // 新しいPhaseデータを追加するためのUI
//        static char newPhaseName[128] = "";
//
//        ImGui::InputText("New Phase Data Name", newPhaseName, _countof(newPhaseName));
//
//        if (ImGui::Button("Add Phase Data"))
//        {
//            if (newPhaseName[0] != '\0')
//            {
//                phaseDatas_[newPhaseName] = Phase{};
//                newPhaseName[0] = '\0'; // フィールドをリセット
//            }
//        }
//
//        if (ImGui::Button("Remove Last Phase Data"))
//        {
//            if (!phaseDatas_.empty())
//            {
//                phaseDatas_.erase(std::prev(phaseDatas_.end())->first);
//            }
//        }
//
//        ImGui::End();
//    }
//}

void AnimationStateManager::LoadFiles()
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

    //phaseDatas_のクリア
    phaseDatas_.clear();

    //JSONデータをphaseDatas_に変換
    for (const auto& [stateName, phasesJson] : root.items())
    {
        AnimationState state;
        state.name = stateName;

        for (const auto& [phaseName, animationTime] : phasesJson.items())
        {
            state.phases.push_back({ phaseName, animationTime });
        }

        phaseDatas_.push_back(state);
    }
}

void AnimationStateManager::SaveFiles()
{
    nlohmann::json root;
    //nlohmann::json型のデータコンテナにグループの全データをまとめる
    root = nlohmann::json::object();

    //phaseDatas_のデータをJSONに変換
    for (const auto& state : phaseDatas_)
    {
        nlohmann::json stateJson = nlohmann::json::object();
        for (const auto& phase : state.phases)
        {
            stateJson[phase.name] = phase.animationTime;
        }
        root[state.name] = stateJson;
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
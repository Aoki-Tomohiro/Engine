#include "CombatAnimationEditor.h"

void CombatAnimationEditor::Initialize()
{
    //設定ファイルを読み込む
    LoadConfigFile();
}

void CombatAnimationEditor::Update()
{
    //ImGuiウィンドウの開始
    ImGui::Begin("CombatAnimationEditor", nullptr, ImGuiWindowFlags_MenuBar);
    //メニューバーの開始
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            //Saveメニュー項目が選択された場合、設定ファイルを保存
            if (ImGui::MenuItem("Save"))
            {
                SaveConfigFile();
            }
            //Loadメニュー項目が選択された場合、設定ファイルを読み込み
            if (ImGui::MenuItem("Load"))
            {
                LoadConfigFile();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    static int selectedStateIndex = -1; //現在選択されている状態のインデックス
    static int selectedPhaseIndex = -1; //現在選択されているフェーズのインデックス

    //新しいアニメーション状態の追加
    static char newStateName[128] = ""; //新しい状態の名前入力用バッファ
    ImGui::InputText("NewAnimationStateName", newStateName, sizeof(newStateName));
    if (ImGui::Button("AddAnimationState"))
    {
        std::string newState(newStateName);
        if (!newState.empty())
        {
            //状態名の重複チェック
            bool stateExists = std::any_of(animationStates_.begin(), animationStates_.end(), [&newState](const CombatAnimationState& state) {
                return state.name == newState;
                });

            if (!stateExists)
            {
                //新しい状態をリストに追加
                animationStates_.push_back({ newState, {} });
                //名前入力バッファをクリア
                std::memset(newStateName, 0, sizeof(newStateName));
            }
        }
    }

    //アニメーション状態の選択と表示
    for (size_t i = 0; i < animationStates_.size(); ++i)
    {
        CombatAnimationState& state = animationStates_[i];

        //フェーズを持続時間順にソート
        std::sort(state.phases.begin(), state.phases.end(), [](const CombatPhase& a, const CombatPhase& b) {
            return a.duration < b.duration;
            });

        //アニメーション状態のノードを展開
        if (ImGui::TreeNode(state.name.c_str()))
        {
            //選択された状態のインデックスを更新
            selectedStateIndex = static_cast<int>(i);

            //新しいフェーズの追加
            static char newPhaseName[128] = ""; //新しいフェーズの名前入力用バッファ
            ImGui::InputText("NewPhaseName", newPhaseName, sizeof(newPhaseName));
            if (ImGui::Button("AddPhase"))
            {
                std::string newPhase(newPhaseName);
                if (!newPhase.empty())
                {
                    //フェーズ名の重複チェック
                    bool phaseExists = std::any_of(state.phases.begin(), state.phases.end(), [&newPhase](const CombatPhase& phase) {
                        return phase.name == newPhase;
                        });

                    if (!phaseExists)
                    {
                        //新しいフェーズをリストに追加
                        state.phases.push_back({ newPhase, 0.0f });
                        std::memset(newPhaseName, 0, sizeof(newPhaseName));
                    }
                }
            }

            //フェーズの表示と編集
            for (size_t j = 0; j < state.phases.size(); ++j)
            {
                CombatPhase& phase = state.phases[j];
                //フェーズのノードを展開
                if (ImGui::TreeNode(phase.name.c_str()))
                {
                    //選択されたフェーズのインデックスを更新
                    selectedPhaseIndex = static_cast<int>(j);

                    //フェーズの持続時間を編集
                    ImGui::DragFloat("Duration", &phase.duration, 0.01f);

                    //攻撃設定の表示
                    if (ImGui::CollapsingHeader("AttackSettings"))
                    {
                        ImGui::DragFloat("MoveSpeed", &phase.attackSettings.moveSpeed, 0.01f);
                        ImGui::DragInt("HitCount", &phase.attackSettings.hitCount, 1, 0);
                        ImGui::DragFloat("HitInterval", &phase.attackSettings.hitInterval, 0.01f);
                        ImGui::DragFloat("Damage", &phase.attackSettings.damage, 0.01f);
                    }

                    //ヒットボックスの表示
                    if (ImGui::CollapsingHeader("Hitbox"))
                    {
                        ImGui::DragFloat3("HitboxCenter", &phase.hitbox.center.x, 0.01f);
                        ImGui::DragFloat3("HitboxSize", &phase.hitbox.size.x, 0.01f);
                    }

                    //ノックバック設定の表示
                    if (ImGui::CollapsingHeader("KnockbackSettings"))
                    {
                        ImGui::DragFloat3("KnockbackVelocity", &phase.knockbackSettings.knockbackVelocity.x, 0.01f);
                        ImGui::DragFloat3("KnockbackAcceleration", &phase.knockbackSettings.knockbackAcceleration.x, 0.01f);
                        ImGui::DragFloat("KnockbackDuration", &phase.knockbackSettings.knockbackDuration, 0.01f);
                    }

                    //エフェクト設定の表示
                    if (ImGui::CollapsingHeader("EffectSettings"))
                    {
                        ImGui::DragFloat("HitStopDuration", &phase.effectSettings.hitStopDuration, 0.01f);
                        ImGui::DragFloat("CameraShakeDuration", &phase.effectSettings.cameraShakeDuration, 0.01f);
                        ImGui::DragFloat3("CameraShakeIntensity", &phase.effectSettings.cameraShakeIntensity.x, 0.01f);
                    }

                    //フェーズのノードを閉じる
                    ImGui::TreePop();

                    //フェーズの削除ボタン
                    if (ImGui::Button("DeletePhase"))
                    {
                        //選択されたフェーズを削除
                        state.phases.erase(state.phases.begin() + j);
                        --j; //削除後のインデックス調整
                    }
                }
            }

            //アニメーション状態の削除ボタン
            if (ImGui::Button("DeleteState"))
            {
                animationStates_.erase(animationStates_.begin() + i); //選択された状態を削除
                --i; //削除後のインデックス調整
                selectedStateIndex = -1; //選択された状態のインデックスをリセット
                selectedPhaseIndex = -1; //選択されたフェーズのインデックスをリセット
            }

            ImGui::TreePop();
        }
    }
    //ImGuiウィンドウの終了
    ImGui::End();
}

const CombatAnimationState CombatAnimationEditor::GetAnimationState(const std::string& name) const
{
    //指定された名前のアニメーション状態を検索して返す
    for (const auto& state : animationStates_)
    {
        if (state.name == name)
        {
            return state;
        }
    }
    return CombatAnimationState();
}

void CombatAnimationEditor::LoadConfigFile()
{
    //ファイルの存在チェック
    if (!std::filesystem::exists(kFileName))
    {
        //デフォルトの内容でファイルを生成
        SaveConfigFile();
        return;
    }

    //読み込み用ファイルストリーム
    std::ifstream ifs;
    //ファイルを読み込み用に開く
    ifs.open(kFileName);

    nlohmann::json root;
    //json文字列からjsonのデータ構造に展開
    ifs >> root;
    //ファイルを閉じる
    ifs.close();

    //現在のアニメーション状態をクリア
    animationStates_.clear();

    //JSONデータをanimationStates_に変換
    for (const auto& [stateName, phasesJson] : root.items())
    {
        CombatAnimationState state;
        state.name = stateName;

        for (const auto& [phaseName, phaseJson] : phasesJson.items())
        {
            CombatPhase phase;
            phase.name = phaseName;
            phase.duration = phaseJson.value("Duration", 0.0f);
            phase.attackSettings.moveSpeed = phaseJson.value("MoveSpeed", 0.0f);
            phase.attackSettings.hitInterval = phaseJson.value("HitInterval", 0.0f);
            phase.attackSettings.hitCount = phaseJson.value("HitCount", 0);
            phase.attackSettings.damage = phaseJson.value("Damage", 0.0f);
            phase.hitbox.center.x = phaseJson.at("HitboxCenter")[0].get<float>();
            phase.hitbox.center.y = phaseJson.at("HitboxCenter")[1].get<float>();
            phase.hitbox.center.z = phaseJson.at("HitboxCenter")[2].get<float>();
            phase.hitbox.size.x = phaseJson.at("HitboxSize")[0].get<float>();
            phase.hitbox.size.y = phaseJson.at("HitboxSize")[1].get<float>();
            phase.hitbox.size.z = phaseJson.at("HitboxSize")[2].get<float>();
            phase.knockbackSettings.knockbackVelocity.x = phaseJson.at("KnockbackVelocity")[0].get<float>();
            phase.knockbackSettings.knockbackVelocity.y = phaseJson.at("KnockbackVelocity")[1].get<float>();
            phase.knockbackSettings.knockbackVelocity.z = phaseJson.at("KnockbackVelocity")[2].get<float>();
            phase.knockbackSettings.knockbackAcceleration.x = phaseJson.at("KnockbackAcceleration")[0].get<float>();
            phase.knockbackSettings.knockbackAcceleration.y = phaseJson.at("KnockbackAcceleration")[1].get<float>();
            phase.knockbackSettings.knockbackAcceleration.z = phaseJson.at("KnockbackAcceleration")[2].get<float>();
            phase.knockbackSettings.knockbackDuration = phaseJson.value("KnockbackDuration", 0.0f);
            phase.effectSettings.hitStopDuration = phaseJson.value("HitStopDuration", 0.0f);
            phase.effectSettings.cameraShakeDuration = phaseJson.value("CameraShakeDuration", 0.0f);
            phase.effectSettings.cameraShakeIntensity.x = phaseJson.at("CameraShakeIntensity")[0].get<float>();
            phase.effectSettings.cameraShakeIntensity.y = phaseJson.at("CameraShakeIntensity")[1].get<float>();
            phase.effectSettings.cameraShakeIntensity.z = phaseJson.at("CameraShakeIntensity")[2].get<float>();
            state.phases.push_back(phase);
        }
        animationStates_.push_back(state);
    }
}

void CombatAnimationEditor::SaveConfigFile()
{
    //現在のアニメーション状態をJSONに変換
    nlohmann::json root = nlohmann::json::object();

    for (const auto& state : animationStates_)
    {
        nlohmann::json stateJson = nlohmann::json::object();
        for (const auto& phase : state.phases)
        {
            nlohmann::json phaseJson = nlohmann::json::object();
            phaseJson["Duration"] = phase.duration;
            phaseJson["MoveSpeed"] = phase.attackSettings.moveSpeed;
            phaseJson["HitInterval"] = phase.attackSettings.hitInterval;
            phaseJson["HitCount"] = phase.attackSettings.hitCount;
            phaseJson["Damage"] = phase.attackSettings.damage;
            phaseJson["HitboxCenter"] = nlohmann::json::array({ phase.hitbox.center.x, phase.hitbox.center.y, phase.hitbox.center.z });
            phaseJson["HitboxSize"] = nlohmann::json::array({ phase.hitbox.size.x, phase.hitbox.size.y, phase.hitbox.size.z });
            phaseJson["KnockbackVelocity"] = nlohmann::json::array({ phase.knockbackSettings.knockbackVelocity.x, phase.knockbackSettings.knockbackVelocity.y, phase.knockbackSettings.knockbackVelocity.z });
            phaseJson["KnockbackAcceleration"] = nlohmann::json::array({ phase.knockbackSettings.knockbackAcceleration.x, phase.knockbackSettings.knockbackAcceleration.y, phase.knockbackSettings.knockbackAcceleration.z });
            phaseJson["KnockbackDuration"] = phase.knockbackSettings.knockbackDuration;
            phaseJson["HitStopDuration"] = phase.effectSettings.hitStopDuration;
            phaseJson["CameraShakeDuration"] = phase.effectSettings.cameraShakeDuration;
            phaseJson["CameraShakeIntensity"] = nlohmann::json::array({ phase.effectSettings.cameraShakeIntensity.x, phase.effectSettings.cameraShakeIntensity.y, phase.effectSettings.cameraShakeIntensity.z });
            stateJson[phase.name] = phaseJson;
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
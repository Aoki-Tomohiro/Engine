#include "CombatAnimationEditor.h"

void CombatAnimationEditor::Initialize()
{

}

void CombatAnimationEditor::Update()
{
    // アニメーションのタイムラインを表現するスクロールバー
    static float animTime = 0.0f;
    static float animLength = 10.0f; // アニメーションの長さ（例: 10秒）
    static std::vector<float> notifyPoints = { 2.0f, 5.0f }; // 通知ポイントの位置（例: 2秒と5秒）

    // タイムラインスライダーを作成
    ImGui::SliderFloat("Animation Time", &animTime, 0.0f, animLength);

    // 通知ポイントを描画
    ImGui::Text("Notifies:");
    for (size_t i = 0; i < notifyPoints.size(); i++)
    {
        ImGui::SameLine();
        if (ImGui::Button(("Notify " + std::to_string(i + 1)).c_str()))
        {
            // 通知ポイントをクリックした時の処理
            ImGui::OpenPopup(("Notify Editor " + std::to_string(i + 1)).c_str());
        }

        // 通知ポイントの詳細を編集するポップアップウィンドウ
        if (ImGui::BeginPopup(("Notify Editor " + std::to_string(i + 1)).c_str()))
        {
            ImGui::Text("Edit Notify %d", i + 1);
            ImGui::SliderFloat("Notify Time", &notifyPoints[i], 0.0f, animLength);
            ImGui::EndPopup();
        }
    }

    // 新しい通知ポイントを追加するボタン
    if (ImGui::Button("Add Notify"))
    {
        notifyPoints.push_back(animTime);  // 現在のアニメーションタイムに通知を追加
    }
}
/**
 * @file UIManager.cpp
 * @brief 全てのUIを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "UIManager.h"

void UIManager::Initialize(const std::string& fileName)
{
    //ファイルパスの生成
    std::string filePath = "Application/Resources/Config/UI/" + fileName;

    //リソースファイルを読み込む
    std::ifstream file(filePath);
    assert(file.is_open());

    //1行分の文字列を入れる変数
    std::string line{};

    //リソースデータを読み込む
    while (std::getline(file, line))
    {
        //"//"から始まる行またはキーやファイル名が設定されていなければ処理を飛ばす
        if (line.find("//") == 0 || line.empty()) continue;

        //CSV行を分割
        std::vector<std::string> tokens = SplitCSVLine(line);

        //要素を取得
        std::string name = tokens[static_cast<int>(CSVColumns::Name)];
        std::string type = tokens[static_cast<int>(CSVColumns::Type)];
        std::string textureName = tokens[static_cast<int>(CSVColumns::TextureName)];
        Vector2 position = { std::stof(tokens[static_cast<int>(CSVColumns::PositionX)]), std::stof(tokens[static_cast<int>(CSVColumns::PositionY)]) };
        Vector2 scale = { std::stof(tokens[static_cast<int>(CSVColumns::ScaleX)]), std::stof(tokens[static_cast<int>(CSVColumns::ScaleY)]) };
        Vector2 anchorPoint = { std::stof(tokens[static_cast<int>(CSVColumns::AnchorX)]), std::stof(tokens[static_cast<int>(CSVColumns::AnchorY)]) };

        //タイプごとの処理を分ける
        if (type == "Static")
        {
            AddStaticUI(name, textureName, position, scale, anchorPoint);
        }
        else if (type == "Interactive") 
        {
            AddInteractiveUI(name, textureName, position, scale, tokens, anchorPoint);
        }
        else if (type == "Dynamic") 
        {
            AddDynamicUI(name, textureName, position, scale, tokens, anchorPoint);
        }
    }

    //ファイルを閉じる
    file.close();
}

void UIManager::Update()
{
    //ImGui開始
    ImGui::Begin("UIManager");

    //全てのUIの更新
    for (const auto& name : uiOrder_)
    {
        //UIを取得
        auto& uiElement = uiElements_[name];

        //TreeNode開始
        if (ImGui::TreeNode(name.c_str()))
        {
            //描画フラグを設定
            bool isVisible = uiElement->GetIsVisible();
            ImGui::Checkbox("IsVisible", &isVisible);
            uiElement->SetIsVisible(isVisible);

            //座標を設定
            Vector2 position = uiElement->GetPosition();
            ImGui::DragFloat2("Position", &position.x);
            uiElement->SetPosition(position);

            //スケールを設定
            Vector2 scale = uiElement->GetScale();
            ImGui::DragFloat2("Scale", &scale.x, 0.01f);
            uiElement->SetScale(scale);

            //TreeNodeを閉じる
            ImGui::TreePop();
        }

        //UIの更新
        uiElement->Update();
    }

    //ImGui終了
    ImGui::End();
}

void UIManager::Draw()
{
    //全てのUIの描画（追加順に処理）
    for (const auto& name : uiOrder_)
    {
        uiElements_[name]->Draw();
    }
}

void UIManager::AddUI(const std::string& uiName, std::unique_ptr<UIElement> uiElement)
{
    //UIの追加
    if (uiElements_.find(uiName) == uiElements_.end())
    {
        uiOrder_.push_back(uiName);
    }
    uiElements_[uiName] = std::move(uiElement);
}

void UIManager::AddStaticUI(const std::string& name, const std::string& textureName, const Vector2& position, const Vector2& scale, const Vector2& anchorPoint)
{
    //UIの追加
    AddUI(name, std::make_unique<UIElement>(textureName, position, scale, anchorPoint));
}

void UIManager::AddInteractiveUI(const std::string& name, const std::string& textureName, const Vector2& position, const Vector2& scale, const std::vector<std::string>& tokens, const Vector2& anchorPoint)
{
    //ボタンのタイプを入れる配列
    std::vector<InteractiveUI::ButtonType> buttonTypes;
    
    //ボタンタイプのパラメーターを取得
    for (size_t i = static_cast<int>(CSVColumns::ParameterStart); i < tokens.size(); ++i) 
    {
        buttonTypes.push_back(ParseButtonType(tokens[i]));
    }

    //UIの追加
    AddUI(name, std::make_unique<InteractiveUI>(textureName, position, scale, buttonTypes, anchorPoint));
}

void UIManager::AddDynamicUI(const std::string& name, const std::string& textureName, const Vector2& position, const Vector2& scale, const std::vector<std::string>& tokens, const Vector2& anchorPoint)
{
    //パラメーターが設定されているかを確認
    if (tokens.size() <= static_cast<int>(CSVColumns::ParameterStart)) return;

    //パラメーターを取得
    float parameter = std::stof(tokens[static_cast<int>(CSVColumns::ParameterStart)]);

    //UIの追加
    AddUI(name, std::make_unique<DynamicUI>(textureName, position, scale, parameter, anchorPoint));
}

std::vector<std::string> UIManager::SplitCSVLine(const std::string& line)
{
    //各要素を入れる配列
    std::vector<std::string> tokens{};
    
    //一つ分の要素を入れる変数
    std::string token;

    //文字列をカンマで区切って読み取るためのストリーム
    std::istringstream tokenStream(line);

    //カンマで区切られた部分ごとに要素を取り出す
    while (std::getline(tokenStream, token, ','))
    {
        tokens.push_back(token);
    }

    //分割された要素のベクターを返す
    return tokens;
}

InteractiveUI::ButtonType UIManager::ParseButtonType(const std::string& buttonTypeStr)
{
    if (buttonTypeStr == "ButtonA") return InteractiveUI::ButtonType::ButtonA;
    if (buttonTypeStr == "ButtonB") return InteractiveUI::ButtonType::ButtonB;
    if (buttonTypeStr == "ButtonX") return InteractiveUI::ButtonType::ButtonX;
    if (buttonTypeStr == "ButtonY") return InteractiveUI::ButtonType::ButtonY;
    if (buttonTypeStr == "ButtonLB") return InteractiveUI::ButtonType::ButtonLB;
    if (buttonTypeStr == "ButtonRB") return InteractiveUI::ButtonType::ButtonRB;
    if (buttonTypeStr == "ButtonLT") return InteractiveUI::ButtonType::ButtonLT;
    if (buttonTypeStr == "ButtonRT") return InteractiveUI::ButtonType::ButtonRT;
    return InteractiveUI::ButtonType::None;
}
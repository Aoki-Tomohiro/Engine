/**
 * @file InteractiveUI.cpp
 * @brief インタラクティブなUIを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "InteractiveUI.h"

void InteractiveUI::Update()
{
    //スケール変更の倍率
    static const float kScaleFactor = 1.2f;

    //補間の速度
    static const float kInterpolationSpeed = 0.4f;

    //目標のスケールを設定
    Vector2 targetScale = AreAllButtonsPressed() ? baseScale_ * kScaleFactor : baseScale_;

    //現在のスケールを取得
    scale_ = Mathf::Lerp(scale_, targetScale, kInterpolationSpeed);

    //基底クラスの呼び出し
    UIElement::Update();
}

bool InteractiveUI::AreAllButtonsPressed()
{
    for (const auto& buttonType : buttonTypes_)
    {
        //ボタン入力を検出
        if (!IsButtonPressed(buttonType))
        {
            return false;
        }
    }
    //全て押されている場合
    return true; 
}

bool InteractiveUI::IsButtonPressed(const ButtonType& buttonType)
{
    //トリガー入力の閾値
    static const float kTriggerThreshold = 0.7f;

    //ボタンタイプに応じた処理
    switch (buttonType)
    {
    case ButtonType::ButtonA: return input_->IsPressButton(XINPUT_GAMEPAD_A);
        break;
    case ButtonType::ButtonB: return input_->IsPressButton(XINPUT_GAMEPAD_B);
        break;
    case ButtonType::ButtonX: return input_->IsPressButton(XINPUT_GAMEPAD_X);
        break;
    case ButtonType::ButtonY: return input_->IsPressButton(XINPUT_GAMEPAD_Y);
        break;
    case ButtonType::ButtonLB: return input_->IsPressButton(XINPUT_GAMEPAD_LEFT_SHOULDER);
        break;
    case ButtonType::ButtonRB: return input_->IsPressButton(XINPUT_GAMEPAD_RIGHT_SHOULDER);
        break;
    case ButtonType::ButtonLT: return input_->GetLeftTriggerValue() > kTriggerThreshold;
        break;
    case ButtonType::ButtonRT: return input_->GetRightTriggerValue() > kTriggerThreshold;
        break;
    }
    return false;
}
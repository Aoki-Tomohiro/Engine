/**
 * @file InteractiveUI.h
 * @brief インタラクティブなUIを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "UIElement.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include <functional>

class InteractiveUI : public UIElement
{
public:
	//ボタンのタイプ
	enum class ButtonType
	{
		None,
		ButtonA,
		ButtonB,
		ButtonX,
		ButtonY,
		ButtonLB,
		ButtonRB,
		ButtonLT,
		ButtonRT,
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="textureName">テクスチャの名前</param>
	/// <param name="position">座標</param>
	/// <param name="scale">スケール</param>
	/// <param name="buttonTypes">ボタンのタイプ</param>
	/// <param name="anchorPoint">アンカーポイント</param>	
	InteractiveUI(const std::string& textureName, const Vector2& position, const Vector2& scale, const std::vector<ButtonType>& buttonTypes, const Vector2& anchorPoint = { 0.5f,0.5f }) :
		UIElement(textureName, position, scale, anchorPoint), baseScale_(scale), buttonTypes_(buttonTypes), input_(Input::GetInstance()) {};

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private:
	/// <summary>
	/// 全てのボタンが押されているかを確認
	/// </summary>
	/// <returns>全てのボタンが押されているか</returns>
	bool AreAllButtonsPressed();

	/// <summary>
	/// ボタンが押されているかを確認
	/// </summary>
	/// <param name="buttonType">ボタンのタイプ</param>
	/// <returns>ボタンが押されているか</returns>
	bool IsButtonPressed(const ButtonType& buttonType);

private:
	//インプット
	Input* input_ = nullptr;

	//基本のスケール
	Vector2 baseScale_ = { 1.0f, 1.0f };

	//必要なボタンのリスト
	std::vector<ButtonType> buttonTypes_{};
};


/**
 * @file UIElement.cpp
 * @brief UIの基底クラスを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "UIElement.h"

UIElement::UIElement(const std::string& textureName, const Vector2& position, const Vector2& scale, const Vector2& anchorPoint)
{
	//テクスチャの名前を設定
	textureName_ = textureName;

	//テクスチャを読み込む
	TextureManager::Load(textureName);

	//座標を設定
	position_ = position;

	//スプライトの生成
	sprite_.reset(Sprite::Create(textureName, position_));

	//スケールを設定
	scale_ = scale;

	//スケールの初期化
	sprite_->SetScale(scale_);

	//アンカーポイントを初期化
	anchorPoint_ = anchorPoint;

	//アンカーポイントを設定
	sprite_->SetAnchorPoint(anchorPoint_);
}

void UIElement::Update()
{
	//座標を更新
	sprite_->SetPosition(position_);

	//スケールを更新
	sprite_->SetScale(scale_);
}

void UIElement::Draw()
{
	//描画フラグが立っていない場合は処理を飛ばす
	if (!isVisible_) return;

	//スプライトの描画
	sprite_->Draw();
}
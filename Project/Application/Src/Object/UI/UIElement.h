/**
 * @file UIElement.h
 * @brief UIの基底クラスを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/2D/Sprite.h"

class UIElement
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="textureName">テクスチャの名前</param>
	/// <param name="position">座標</param>
	/// <param name="scale">スケール</param>
	/// <param name="anchorPoint">アンカーポイント</param>	
	UIElement(const std::string& textureName, const Vector2& position, const Vector2& scale, const Vector2& anchorPoint = { 0.5f, 0.5f });

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~UIElement() = default;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//座標を取得・設定
	const Vector2& GetPosition() const { return position_; };
	void SetPosition(const Vector2& position) { position_ = position; };

	//スケールを取得・設定
	const Vector2& GetScale() const { return scale_; };
	void SetScale(const Vector2& scale) { scale_ = scale; };

	//アンカーポイントを取得・設定
	const Vector2& GetAnchorPoint() const { return anchorPoint_; };
	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; };

	//描画フラグを取得・設定
	const bool GetIsVisible() const { return isVisible_; };
	void SetIsVisible(const bool isVisible) { isVisible_ = isVisible; };

	//スプライトを取得
	Sprite* GetSprite() const { return sprite_.get(); };

protected:
	//スプライト
	std::unique_ptr<Sprite> sprite_ = nullptr;

	//テクスチャの名前
	std::string textureName_{};

	//座標
	Vector2 position_{};

	//スケール
	Vector2 scale_ = { 1.0f, 1.0f };

	//アンカーポイント
	Vector2 anchorPoint_ = { 0.5f, 0.5f };

	//描画フラグ
	bool isVisible_ = true;
};


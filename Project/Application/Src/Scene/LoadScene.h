/**
 * @file LoadScene.h
 * @brief ロードシーンを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/2D/Sprite.h"

class LoadScene : public IScene
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// UIの描画
	/// </summary>
	void DrawUI() override;

private:
	Renderer* renderer_ = nullptr;

	std::unique_ptr<Sprite> loadSprite_ = nullptr;

	int timer_ = 0;

	int animationCount_ = 0;
};


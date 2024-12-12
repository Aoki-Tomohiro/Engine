/**
 * @file IScene.h
 * @brief シーンの基底クラス
 * @author 青木智滉
 * @date
 */

#pragma once

class SceneManager;

class IScene
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IScene() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// UIを描画
	/// </summary>
	virtual void DrawUI() = 0;

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Finalize() = 0;

	//シーンマネージャーを設定
	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; };

protected:
	//シーンマネージャー
	SceneManager* sceneManager_ = nullptr;
};
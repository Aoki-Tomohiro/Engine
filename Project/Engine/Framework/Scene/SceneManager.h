/**
 * @file SceneManager.h
 * @brief シーンを管理するクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "AbstractSceneFactory.h"
#include "IScene.h"

class SceneManager
{
public:
	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static SceneManager* GetInstance();

	/// <summary>
	/// 破棄処理
	/// </summary>
	static void Destroy();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// UIの描画
	/// </summary>
	void DrawUI();

	/// <summary>
	/// 読み込み
	/// </summary>
	void Load();

	/// <summary>
	/// シーンを変更
	/// </summary>
	/// <param name="sceneName">シーンの名前</param>
	void ChangeScene(const std::string& sceneName);

	/// <summary>
	/// シーンファクトリーを設定
	/// </summary>
	/// <param name="sceneFactory">シーンファクトリー</param>
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; };

	//ロード画面を表示しているかを取得
	const bool GetLoadingScreenVisible() const { return loadingScreenVisible_; };

private:
	SceneManager() = default;
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	const SceneManager& operator=(const SceneManager&) = delete;

private:
	//インスタンス
	static SceneManager* instance_;

	//現在のシーン
	IScene* currentScene_ = nullptr;

	//次のシーン
	IScene* nextScene_ = nullptr;

	//ロードシーン
	IScene* loadScene_ = nullptr;

	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;

	//ロード画面を表示しているかどうか
	bool loadingScreenVisible_ = false;
};


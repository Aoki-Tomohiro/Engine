#pragma once
#include "AbstractSceneFactory.h"
#include "IScene.h"

class SceneManager
{
public:
	static SceneManager* GetInstance();

	static void Destroy();

	void Update();

	void Draw();

	void DrawUI();

	void Load();

	void ChangeScene(const std::string& sceneName);

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; };

	const bool GetIsLoading() const { return isLoading_; };

	void SetIsLoading(const bool isLoading) { isLoading_ = isLoading; };

private:
	SceneManager() = default;
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	const SceneManager& operator=(const SceneManager&) = delete;

private:
	static SceneManager* instance_;

	IScene* currentScene_ = nullptr;

	IScene* nextScene_ = nullptr;

	IScene* loadScene_ = nullptr;

	AbstractSceneFactory* sceneFactory_ = nullptr;

	bool isLoading_ = false;
};


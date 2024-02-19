#include "SceneManager.h"
#include <cassert>

SceneManager* SceneManager::instance_ = nullptr;

SceneManager* SceneManager::GetInstance() 
{
	if (instance_ == nullptr)
	{
		instance_ = new SceneManager();
	}
	return instance_;
}

void SceneManager::Destroy()
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void SceneManager::Update()
{
	isLoading_ ? loadScene_->Update() : currentScene_->Update();
}

void SceneManager::Draw()
{
	isLoading_ ? loadScene_->Draw() : currentScene_->Draw();
}

void SceneManager::DrawUI()
{
	isLoading_ ? loadScene_->DrawUI() : currentScene_->DrawUI();
}

void SceneManager::Load()
{
	if (nextScene_)
	{
		//旧シーンの終了
		if (currentScene_)
		{
			currentScene_->Finalize();
			delete currentScene_;
		}

		//シーン切り替え
		currentScene_ = nextScene_;
		nextScene_ = nullptr;

		//シーンマネージャーをセット
		currentScene_->SetSceneManager(this);

		//シーンの初期化
		currentScene_->Initialize();

		//ロードフラグをfalseにする
		isLoading_ = false;
	}
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);
	if (!loadScene_)
	{
		loadScene_ = sceneFactory_->CreateScene("LoadScene");
		loadScene_->Initialize();
	}
	nextScene_ = sceneFactory_->CreateScene(sceneName);
	isLoading_ = true;
}

SceneManager::~SceneManager()
{
	currentScene_->Finalize();
	delete currentScene_;
	loadScene_->Finalize();
	delete loadScene_;
}
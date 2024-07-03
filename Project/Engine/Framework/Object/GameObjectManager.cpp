#include "GameObjectManager.h"
#include <cassert>

//実体定義
GameObjectManager* GameObjectManager::instance_ = nullptr;

GameObjectManager* GameObjectManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new GameObjectManager();
	}
	return instance_;
}

void GameObjectManager::Destroy()
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void GameObjectManager::Update()
{
	//破壊フラグが立ったゲームオブジェクトを削除
	std::vector<std::unique_ptr<GameObject>>::iterator it = std::remove_if(gameObjects_.begin(), gameObjects_.end(),
		[](std::unique_ptr<GameObject>& gameObject)
		{
			return gameObject->GetIsDestroy();
		});
	gameObjects_.erase(it, gameObjects_.end());

	//ゲームオブジェクトの更新
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects_)
	{
		if (gameObject->GetIsActive())
		{
			gameObject->Update();
		}
	}
}

void GameObjectManager::Draw()
{
	//カメラがない場合は止める
	assert(camera_);

	//ゲームオブジェクトの描画
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects_)
	{
		if (gameObject->GetIsVisible())
		{
			gameObject->Draw(*camera_);
		}
	}
}

void GameObjectManager::Clear()
{
	//ゲームオブジェクトをクリア
	gameObjects_.clear();
}

GameObject* GameObjectManager::CreateGameObject(const std::string& objectName)
{
	GameObject* newObject = GameObjectManager::GetInstance()->CreateGameObjectInternal(objectName);
	return newObject;
}

GameObject* GameObjectManager::CreateGameObjectInternal(const std::string& objectName)
{
	assert(gameObjectFactory_);
	GameObject* newGameObject = gameObjectFactory_->CreateGameObject(objectName);
	newGameObject->Initialize();
	newGameObject->SetGameObjectManager(this);
	gameObjects_.push_back(std::unique_ptr<GameObject>(newGameObject));
	return newGameObject;
}
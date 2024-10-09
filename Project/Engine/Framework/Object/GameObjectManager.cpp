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

GameObject* GameObjectManager::CreateGameObject(const std::string& objectName)
{
	GameObject* newObject = GameObjectManager::GetInstance()->CreateGameObjectInternal(objectName);
	return newObject;
}

void GameObjectManager::Clear()
{
	//ゲームオブジェクトをクリア
	gameObjects_.clear();
}

void GameObjectManager::Update()
{
	//破壊フラグの立ったゲームオブジェクトを削除
	RemoveDestroyedGameObjects();

	//全ての保留中のゲームオブジェクトを初期化
	ProcessPendingGameObjects();

	//ゲームオブジェクトの更新
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects_)
	{
		if (gameObject->GetIsActive())
		{
			gameObject->Update();
		}
	}
}

void GameObjectManager::Draw(const Camera& camera)
{
	//ゲームオブジェクトの描画
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects_)
	{
		if (gameObject->GetIsVisible())
		{
			gameObject->Draw(camera);
		}
	}
}

GameObject* GameObjectManager::CreateGameObjectInternal(const std::string& objectName)
{
	//ゲームオブジェクトファクトリが存在することを確認
	assert(gameObjectFactory_);

	//ゲームオブジェクトの生成
	GameObject* newGameObject = gameObjectFactory_->CreateGameObject(objectName);

	//ゲームオブジェクトに名前を設定
	newGameObject->SetName(objectName);

	//ゲームオブジェクトマネージャーを設定
	newGameObject->SetGameObjectManager(this);

	//TransformComponentを追加
	newGameObject->AddComponent<TransformComponent>();

	//保留中のゲームオブジェクトに追加
	pendingGameObjects_.push_back(std::unique_ptr<GameObject>(newGameObject));

	//新規ゲームオブジェクトを返す
	return newGameObject;
}

void GameObjectManager::RemoveDestroyedGameObjects()
{
	//破壊フラグが立ったゲームオブジェクトを削除
	std::vector<std::unique_ptr<GameObject>>::iterator it = std::remove_if(gameObjects_.begin(), gameObjects_.end(),
		[](std::unique_ptr<GameObject>& gameObject)
{
			return gameObject->GetIsDestroy();
		});
	gameObjects_.erase(it, gameObjects_.end());
}

void GameObjectManager::ProcessPendingGameObjects()
{
	//保留中のゲームオブジェクトをすべて初期化
	for (const auto& pendingGameObject : pendingGameObjects_)
	{
		pendingGameObject->Initialize();
	}
	//保留中のゲームオブジェクトをゲームオブジェクトリストに追加
	gameObjects_.insert(gameObjects_.end(), std::make_move_iterator(pendingGameObjects_.begin()), std::make_move_iterator(pendingGameObjects_.end()));
	//保留中のゲームオブジェクトのリストをクリア
	pendingGameObjects_.clear();
}
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
	//破壊フラグの立ったオブジェクトを削除
	RemoveDestroyedObjects();

	//新規オブジェクトの初期化と追加
	InitializeNewObjects();

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
	//ゲームオブジェクトの描画
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects_)
	{
		//カメラがなければ止める
		assert(camera_);

		if (gameObject->GetIsVisible())
		{
			gameObject->Draw(*camera_);
		}
	}
}

void GameObjectManager::DrawUI()
{
	//ゲームオブジェクトの描画
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects_)
	{
		gameObject->DrawUI();
	}
}

void GameObjectManager::Clear()
{
	gameObjects_.clear();
	newGameObjectsBuffer_.clear();
	gameObjectPool_.clear();
}

GameObject* GameObjectManager::CreateGameObject(const std::string& objectName)
{
	GameObject* newObject = GameObjectManager::GetInstance()->CreateGameObjectInternal(objectName);
	return newObject;
}

GameObject* GameObjectManager::CreateGameObjectInternal(const std::string& objectName)
{
	//ファクトリが設定されていない場合は止める
	assert(gameObjectFactory_);

	//ファクトリを使ってゲームオブジェクトを生成
	GameObject* newGameObject = gameObjectFactory_->CreateGameObject(objectName);
	newGameObject->SetGameObjectManager(this);
	newGameObject->AddComponent<TransformComponent>();
	newGameObjectsBuffer_.emplace_back(std::unique_ptr<GameObject>(newGameObject));
	return newGameObject;
}

void GameObjectManager::RemoveDestroyedObjects()
{
	//破壊フラグが立ったオブジェクトを検索して削除
	auto it = std::remove_if(gameObjects_.begin(), gameObjects_.end(),
		[this](std::unique_ptr<GameObject>& gameObject) {
			if (gameObject->GetIsDestroy())
			{
				//プールにオブジェクトを戻す
				ReleaseObjectToPool(std::move(gameObject));
				return true;
			}
			return false;
		});
	//削除対象をリストから削除
	gameObjects_.erase(it, gameObjects_.end());
}

void GameObjectManager::InitializeNewObjects()
{
	//新規ゲームオブジェクトを初期化
	for (const auto& newGameObject : newGameObjectsBuffer_)
	{
		newGameObject->Initialize();
	}

	//初期化された新規オブジェクトをゲームオブジェクトリストに追加
	gameObjects_.insert(gameObjects_.end(),
		std::make_move_iterator(newGameObjectsBuffer_.begin()),
		std::make_move_iterator(newGameObjectsBuffer_.end()));

	//新規オブジェクトバッファをクリア
	newGameObjectsBuffer_.clear();
}
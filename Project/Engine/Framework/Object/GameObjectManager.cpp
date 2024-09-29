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
	//gameObjects_のすべてのオブジェクトをプールに戻す
	for (auto& gameObject : gameObjects_)
	{
		ReleaseObjectToPool(std::move(gameObject));
	}

	//newGameObjectsBuffer_のすべてのオブジェクトをプールに戻す
	for (auto& newGameObject : newGameObjectsBuffer_)
	{
		ReleaseObjectToPool(std::move(newGameObject));
	}

	//reusedObjectsBuffer_のすべてのオブジェクトをプールに戻す
	for (auto& newGameObject : reusedObjectsBuffer_)
	{
		ReleaseObjectToPool(std::move(newGameObject));
	}

	//gameObjects_とnewGameObjectsBuffer_とreusedObjectsBuffer_をクリア
	gameObjects_.clear();
	newGameObjectsBuffer_.clear();
	reusedObjectsBuffer_.clear();
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

	//オブジェクトプールから指定された型のオブジェクトを取得
	auto it = gameObjectPool_.find(std::type_index(typeid(*newGameObject)));

	//プールから再利用可能なオブジェクトが存在する場合
	if (it != gameObjectPool_.end() && !it->second.empty())
	{
		delete newGameObject; // 新しいオブジェクトは不要
		GameObject* reusedObject = it->second.front().release(); //プールからオブジェクトを取得
		it->second.pop();

		reusedObject->SetGameObjectManager(this);
		reusedObject->SetIsDestroy(false);
		reusedObjectsBuffer_.emplace_back(reusedObject);

		return reusedObject; //再利用したオブジェクトを返す
	}

	//プールに再利用可能なオブジェクトがない場合、新規作成
	newGameObject->AddComponent<TransformComponent>();
	newGameObject->SetGameObjectManager(this);
	newGameObjectsBuffer_.emplace_back(newGameObject);

	return newGameObject; //新規作成したオブジェクトを返す
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
		newGameObject->Initialize(); //新しく生成されたオブジェクトの初期化
	}

	//プールから取得したゲームオブジェクトをリセット
	for (const auto& reusedObject : reusedObjectsBuffer_)
	{
		reusedObject->Reset(); //プールから取得したオブジェクトのリセット
	}

	//両方のバッファをゲームオブジェクトリストに追加
	gameObjects_.insert(gameObjects_.end(),
		std::make_move_iterator(newGameObjectsBuffer_.begin()),
		std::make_move_iterator(newGameObjectsBuffer_.end()));

	gameObjects_.insert(gameObjects_.end(),
		std::make_move_iterator(reusedObjectsBuffer_.begin()),
		std::make_move_iterator(reusedObjectsBuffer_.end()));

	//両方のバッファをクリア
	newGameObjectsBuffer_.clear();
	reusedObjectsBuffer_.clear();
}
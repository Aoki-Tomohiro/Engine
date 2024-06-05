#include "GameObjectManager.h"

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
	std::vector<std::unique_ptr<IGameObject>>::iterator it = std::remove_if(gameObjects_.begin(), gameObjects_.end(),
		[](std::unique_ptr<IGameObject>& gameObject)
		{
			return gameObject->GetIsDestroy();
		});
	gameObjects_.erase(it, gameObjects_.end());

	//ゲームオブジェクトの更新
	for (std::unique_ptr<IGameObject>& gameObject : gameObjects_)
	{
		if (gameObject->GetIsActive())
		{
			gameObject->Update();
		}
	}

	//カメラの更新
	for (std::unique_ptr<Camera>& camera : cameras_)
	{
		camera->UpdateMatrix();
	}
}

void GameObjectManager::Draw(const Camera& camera)
{
	//ゲームオブジェクトの描画
	for (std::unique_ptr<IGameObject>& gameObject : gameObjects_)
	{
		if (gameObject->GetIsVisible())
		{
			gameObject->Draw(camera);
		}
	}
}

void GameObjectManager::DrawUI()
{
	//ゲームオブジェクトのUI描画
	for (std::unique_ptr<IGameObject>& gameObject : gameObjects_)
	{
		gameObject->DrawUI();
	}
}

void GameObjectManager::Clear()
{
	//ゲームオブジェクトをクリア
	gameObjects_.clear();
}

IGameObject* GameObjectManager::CreateGameObject(const std::string& objectName)
{
	IGameObject* newObject = GameObjectManager::GetInstance()->CreateGameObjectInternal(objectName);
	return newObject;
}

Camera* GameObjectManager::CreateCamera(const std::string& cameraName)
{
	Camera* newCamera = GameObjectManager::GetInstance()->CreateCameraInternal(cameraName);
	return newCamera;
}

IGameObject* GameObjectManager::CreateGameObjectInternal(const std::string& objectName)
{
	assert(gameObjectFactory_);
	IGameObject* newGameObject = gameObjectFactory_->CreateGameObject(objectName);
	newGameObject->Initialize();
	newGameObject->SetGameObjectManager(this);
	gameObjects_.push_back(std::unique_ptr<IGameObject>(newGameObject));
	return newGameObject;
}

Camera* GameObjectManager::CreateCameraInternal(const std::string& cameraName)
{
	Camera* newCamera = new Camera();
	newCamera->Initialize();
	cameras_.push_back(std::unique_ptr<Camera>(newCamera));
	return newCamera;
}
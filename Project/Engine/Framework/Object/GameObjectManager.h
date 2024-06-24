#pragma once
#include "AbstractGameObjectFactory.h"
#include "GameObject.h"
#include <vector>

class GameObjectManager
{
public:
	static GameObjectManager* GetInstance();

	static void Destroy();

	template <typename Type>
	static Type* CreateGameObject();

	static GameObject* CreateGameObject(const std::string& objectName);

	static Camera* CreateCamera();

	void Update();

	void Draw();

	void Clear();

	void SetGameObjectFactory(AbstractGameObjectFactory* gameObjectFactory) { gameObjectFactory_ = gameObjectFactory; };

	template <typename Type>
	Type* GetGameObject() const;

	template <typename Type>
	std::vector<Type*> GetGameObjects() const;

	Camera* GetCamera() const { return camera_.get(); };

private:
	GameObjectManager() = default;
	~GameObjectManager() = default;
	GameObjectManager(const GameObjectManager&) = delete;
	const GameObjectManager& operator=(const GameObjectManager&) = delete;

	template <typename Type>
	Type* CreateGameObjectInternal();

	GameObject* CreateGameObjectInternal(const std::string& objectName);

	Camera* CreateCameraInternal();

private:
	static GameObjectManager* instance_;

	std::vector<std::unique_ptr<GameObject>> gameObjects_{};

	std::unique_ptr<Camera> camera_{};

	AbstractGameObjectFactory* gameObjectFactory_ = nullptr;
};

template<typename Type>
Type* GameObjectManager::CreateGameObject()
{
	//ゲームオブジェクトを作成
	Type* newObject = GameObjectManager::GetInstance()->CreateGameObjectInternal<Type>();
	return newObject;
}

template<typename Type>
Type* GameObjectManager::CreateGameObjectInternal()
{
	Type* newObject = new Type();
	newObject->Initialize();
	newObject->SetGameObjectManager(this);
	gameObjects_.push_back(std::unique_ptr<GameObject>(newObject));
	return newObject;
}

template <typename Type>
Type* GameObjectManager::GetGameObject() const
{
	for (const auto& gameObject : gameObjects_)
	{
		if (Type* castedObject = dynamic_cast<Type*>(gameObject.get()))
		{
			return castedObject;
		}
	}
	return nullptr;
}

template <typename Type>
std::vector<Type*> GameObjectManager::GetGameObjects() const
{
	std::vector<Type*> gameObjects;
	for (const auto& gameObject : gameObjects_)
	{
		if (Type* castedObject = dynamic_cast<Type*>(gameObject.get()))
		{
			gameObjects.push_back(castedObject);
		}
	}
	return gameObjects;
}
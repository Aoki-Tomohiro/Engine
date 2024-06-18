#pragma once
#include "AbstractGameObjectFactory.h"
#include "GameObject.h"
#include <vector>

class GameObjectManager
{
public:
	static GameObjectManager* GetInstance();

	static void Destroy();

	static GameObject* CreateGameObject(const std::string& objectName);

	static Camera* CreateCamera(const std::string& cameraName);

	void Update();

	void Draw();

	void Clear();

	void SetGameObjectFactory(AbstractGameObjectFactory* gameObjectFactory) { gameObjectFactory_ = gameObjectFactory; };

	template <typename Type>
	Type* GetGameObject(const std::string& tag) const;

	template <typename Type>
	std::vector<Type*> GetGameObjects(const std::string& tag) const;

private:
	GameObjectManager() = default;
	~GameObjectManager() = default;
	GameObjectManager(const GameObjectManager&) = delete;
	const GameObjectManager& operator=(const GameObjectManager&) = delete;

	GameObject* CreateGameObjectInternal(const std::string& objectName);

	Camera* CreateCameraInternal(const std::string& cameraName);

private:
	static GameObjectManager* instance_;

	std::vector<std::unique_ptr<GameObject>> gameObjects_{};

	std::unique_ptr<Camera> camera_{};

	AbstractGameObjectFactory* gameObjectFactory_ = nullptr;
};

template <typename Type>
Type* GameObjectManager::GetGameObject(const std::string& tag) const
{
	//ゲームオブジェクトを探す
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects_)
	{
		if (gameObject->GetTag() == tag)
		{
			return dynamic_cast<Type*>(gameObject.get());
		}
	}
	return nullptr;
}

template <typename Type>
std::vector<Type*> GameObjectManager::GetGameObjects(const std::string& tag) const
{
	//ゲームオブジェクトを探す
	std::vector<Type*> gameObjects{};
	for (const std::unique_ptr<GameObject>& gameObject : gameObjects_)
	{
		if (gameObject->GetTag() == tag) {
			gameObjects.push_back(dynamic_cast<const Type*>(gameObject.get()));
		}
	}
	return gameObjects;
}


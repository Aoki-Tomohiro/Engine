#pragma once
#include "AbstractGameObjectFactory.h"
#include "IGameObject.h"
#include <vector>

class GameObjectManager
{
public:
	static GameObjectManager* GetInstance();

	static void Destroy();

	static IGameObject* CreateGameObject(const std::string& objectName);

	static Camera* CreateCamera(const std::string& cameraName);

	void Update();

	void Draw(const Camera& camera);

	void DrawUI();

	void Clear();

	void SetGameObjectFactory(AbstractGameObjectFactory* gameObjectFactory) { gameObjectFactory_ = gameObjectFactory; };

	template <typename Type>
	static Type* CreateGameObjectFromType();

	template <typename Type>
	Type* GetGameObject(const std::string& tag) const;

	template <typename Type>
	std::vector<Type*> GetGameObjects(const std::string& tag) const;

private:
	GameObjectManager() = default;
	~GameObjectManager() = default;
	GameObjectManager(const GameObjectManager&) = delete;
	const GameObjectManager& operator=(const GameObjectManager&) = delete;

	IGameObject* CreateGameObjectInternal(const std::string& objectName);

	Camera* CreateCameraInternal(const std::string& cameraName);

	template <typename Type>
	Type* CreateGameObjectInternalFromType();

private:
	static GameObjectManager* instance_;

	std::vector<std::unique_ptr<IGameObject>> gameObjects_{};

	std::vector<std::unique_ptr<Camera>> cameras_{};

	AbstractGameObjectFactory* gameObjectFactory_ = nullptr;
};

template <typename Type>
Type* GameObjectManager::CreateGameObjectFromType() {
	//ゲームオブジェクトを作成
	Type* newObject = GameObjectManager::GetInstance()->CreateGameObjectInternalFromType<Type>();
	return newObject;
}

template <typename Type>
Type* GameObjectManager::CreateGameObjectInternalFromType() {
	Type* newObject = new Type();
	newObject->Initialize();
	newObject->SetGameObjectManager(this);
	gameObjects_.push_back(std::unique_ptr<IGameObject>(newObject));
	return newObject;
}

template <typename Type>
Type* GameObjectManager::GetGameObject(const std::string& tag) const
{
	//ゲームオブジェクトを探す
	for (const std::unique_ptr<IGameObject>& gameObject : gameObjects_)
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
	for (const std::unique_ptr<IGameObject>& gameObject : gameObjects_)
	{
		if (gameObject->GetTag() == tag) {
			gameObjects.push_back(dynamic_cast<const Type*>(gameObject.get()));
		}
	}
	return gameObjects;
}
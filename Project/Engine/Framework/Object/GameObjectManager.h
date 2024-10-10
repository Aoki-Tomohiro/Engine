#pragma once
#include "Engine/Framework/Object/AbstractGameObjectFactory.h"
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include <vector>

class GameObjectManager
{
public:
    static GameObjectManager* GetInstance();

    static void Destroy();

    template <typename Type>
    static Type* CreateGameObject(const std::string& objectName);

    static GameObject* CreateGameObject(const std::string& objectName);
    
	void Clear();

    void Update();

	void Draw(const Camera& camera);

	void DrawUI();

	void SetGameObjectFactory(AbstractGameObjectFactory* gameObjectFactory) { gameObjectFactory_ = gameObjectFactory; };

	template <typename Type>
	Type* GetGameObject(const std::string& objectName) const;

	template <typename Type>
	std::vector<Type*> GetGameObjects(const std::string& objectName) const;

private:
    GameObjectManager() = default;
    ~GameObjectManager() = default;
    GameObjectManager(const GameObjectManager&) = delete;
    const GameObjectManager& operator=(const GameObjectManager&) = delete;

    template <typename Type>
	Type* CreateGameObjectInternal(const std::string& objectName);

    GameObject* CreateGameObjectInternal(const std::string& objectName);

    void RemoveDestroyedGameObjects();

    void ProcessPendingGameObjects();

	template <typename Type>
	Type* FindGameObjectByName(const std::vector<std::unique_ptr<GameObject>>& objects, const std::string& objectName) const;

	template <typename Type>
	void CollectGameObjectsByName(const std::vector<std::unique_ptr<GameObject>>& objects, const std::string& objectName, std::vector<Type*>& result) const;

private:
    static GameObjectManager* instance_;

    std::vector<std::unique_ptr<GameObject>> gameObjects_;

    std::vector<std::unique_ptr<GameObject>> pendingGameObjects_;

    AbstractGameObjectFactory* gameObjectFactory_ = nullptr;
};


template<typename Type>
Type* GameObjectManager::CreateGameObject(const std::string& objectName)
{
    //新しいゲームオブジェクトを内部で生成し、返す
    return GetInstance()->CreateGameObjectInternal<Type>(objectName);
}

template<typename Type>
Type* GameObjectManager::CreateGameObjectInternal(const std::string& objectName)
{
    //ゲームオブジェクトを生成
    Type* newObject = new Type();
	newObject->SetName(objectName);
	newObject->SetGameObjectManager(this);
	newObject->AddComponent<TransformComponent>();
    pendingGameObjects_.push_back(std::unique_ptr<GameObject>(newObject));
    return newObject;
}

template <typename Type>
Type* GameObjectManager::GetGameObject(const std::string& objectName) const
{
	//ゲームオブジェクトを検索し名前に一致するものがあれば返す
	if (Type* foundObject = FindGameObjectByName<Type>(pendingGameObjects_, objectName))
	{
		return foundObject;
	}
	return FindGameObjectByName<Type>(gameObjects_, objectName);
}

template <typename Type>
Type* GameObjectManager::FindGameObjectByName(const std::vector<std::unique_ptr<GameObject>>& objects, const std::string& objectName) const
{
	for (const auto& gameObject : objects)
	{
		if (gameObject->GetName() == objectName)
		{
			if (Type* castedObject = dynamic_cast<Type*>(gameObject.get()))
			{
				return castedObject;
			}
		}
	}
	return nullptr;
}

template <typename Type>
std::vector<Type*> GameObjectManager::GetGameObjects(const std::string& objectName) const
{
	std::vector<Type*> gameObjects;
	//保留中のゲームオブジェクトから名前に一致するものを収集
	CollectGameObjectsByName<Type>(pendingGameObjects_, objectName, gameObjects);
	//既存のゲームオブジェクトからも収集
	CollectGameObjectsByName<Type>(gameObjects_, objectName, gameObjects);
	return gameObjects;
}

template <typename Type>
void GameObjectManager::CollectGameObjectsByName(const std::vector<std::unique_ptr<GameObject>>& objects, const std::string& objectName, std::vector<Type*>& result) const
{
	for (const auto& gameObject : objects)
	{
		if (gameObject->GetName() == objectName)
		{
			if (Type* castedObject = dynamic_cast<Type*>(gameObject.get()))
			{
				result.push_back(castedObject);
			}
		}
	}
}
#pragma once
#include "AbstractGameObjectFactory.h"
#include "GameObject.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include <vector>

class GameObjectManager
{
public:
	static GameObjectManager* GetInstance();

	static void Destroy();

	template <typename Type>
	static Type* CreateGameObject();

	static GameObject* CreateGameObject(const std::string& objectName);

	void Update();

	void Draw();

	void DrawUI();

	void Clear();

	void SetCamera(const Camera* camera) { camera_ = camera; };

	void SetGameObjectFactory(AbstractGameObjectFactory* gameObjectFactory) { gameObjectFactory_ = gameObjectFactory; };

	template <typename Type>
	const Type* GetConstGameObject(const std::string& name) const;

	template <typename Type>
	const std::vector<Type*> GetConstGameObjects(const std::string& name) const;

	template <typename Type>
	Type* GetMutableGameObject(const std::string& name) const;

	template <typename Type>
	std::vector<Type*> GetMutableGameObjects(const std::string& name) const;

private:
	GameObjectManager() = default;
	~GameObjectManager() = default;
	GameObjectManager(const GameObjectManager&) = delete;
	const GameObjectManager& operator=(const GameObjectManager&) = delete;

	template <typename Type>
	Type* CreateGameObjectInternal();

	GameObject* CreateGameObjectInternal(const std::string& objectName);

private:
	static GameObjectManager* instance_;

	std::vector<std::unique_ptr<GameObject>> gameObjects_{};

	std::vector<std::unique_ptr<GameObject>> newGameObjectsBuffer_;

	const Camera* camera_ = nullptr;

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
	newObject->SetGameObjectManager(this);
	newObject->AddComponent<TransformComponent>();
	newGameObjectsBuffer_.push_back(std::unique_ptr<GameObject>(newObject));
	return newObject;
}

template <typename Type>
const Type* GameObjectManager::GetConstGameObject(const std::string& name) const
{
	for (const auto& gameObject : newGameObjectsBuffer_)
	{
		if (gameObject->GetName() == name)
		{
			if (Type* castedObject = dynamic_cast<Type*>(gameObject.get()))
			{
				return castedObject;
			}
		}
	}
	for (const auto& gameObject : gameObjects_)
	{
		if (gameObject->GetName() == name)
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
const std::vector<Type*> GameObjectManager::GetConstGameObjects(const std::string& name) const
{
	std::vector<Type*> gameObjects;
	for (const auto& gameObject : newGameObjectsBuffer_)
	{
		if (gameObject->GetName() == name)
		{
			if (Type* castedObject = dynamic_cast<Type*>(gameObject.get()))
			{
				gameObjects.push_back(castedObject);
			}
		}
	}
	for (const auto& gameObject : gameObjects_)
	{
		if (gameObject->GetName() == name)
		{
			if (Type* castedObject = dynamic_cast<Type*>(gameObject.get()))
			{
				gameObjects.push_back(castedObject);
			}
		}
	}
	return gameObjects;
}

template <typename Type>
Type* GameObjectManager::GetMutableGameObject(const std::string& name) const
{
	for (const auto& gameObject : newGameObjectsBuffer_)
	{
		if (gameObject->GetName() == name)
		{
			if (Type* castedObject = dynamic_cast<Type*>(gameObject.get()))
			{
				return castedObject;
			}
		}
	}
	for (const auto& gameObject : gameObjects_)
	{
		if (gameObject->GetName() == name)
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
std::vector<Type*> GameObjectManager::GetMutableGameObjects(const std::string& name) const
{
	std::vector<Type*> gameObjects;
	for (const auto& gameObject : newGameObjectsBuffer_)
	{
		if (gameObject->GetName() == name)
		{
			if (Type* castedObject = dynamic_cast<Type*>(gameObject.get()))
			{
				gameObjects.push_back(castedObject);
			}
		}
	}
	for (const auto& gameObject : gameObjects_)
	{
		if (gameObject->GetName() == name)
		{
			if (Type* castedObject = dynamic_cast<Type*>(gameObject.get()))
			{
				gameObjects.push_back(castedObject);
			}
		}
	}
	return gameObjects;
}
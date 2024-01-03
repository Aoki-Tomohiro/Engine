#pragma once
#include "IGameObject.h"
#include <vector>

class GameObjectManager
{
public:
	static GameObjectManager* GetInstance();

	static void Destroy();

	void Initialize();

	void Update();

	void Draw(const Camera& camera);

	template <typename Type>
	static Type* CreateGameObject();

	const IGameObject* GetGameObject(const std::string& tag) const;

	const std::vector<IGameObject*> GetGameObjects(const std::string& tag) const;

private:
	GameObjectManager() = default;
	~GameObjectManager() = default;
	GameObjectManager(const GameObjectManager&) = delete;
	const GameObjectManager& operator=(const GameObjectManager&) = delete;

	template <typename Type>
	Type* CreateGameObjectInternal();

private:
	static GameObjectManager* instance_;

	std::vector<std::unique_ptr<IGameObject>> gameObjects_{};
};

template <typename Type>
Type* GameObjectManager::CreateGameObject() {
	//ゲームオブジェクトを作成
	Type* newObject = GameObjectManager::GetInstance()->CreateGameObjectInternal<Type>();
	return newObject;
}

template <typename Type>
Type* GameObjectManager::CreateGameObjectInternal() {
	Type* newObject = new Type();
	newObject->Initialize();
	gameObjects_.push_back(std::unique_ptr<IGameObject>(newObject));
	return newObject;
}
#pragma once
#include "AbstractGameObjectFactory.h"
#include "GameObject.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include <queue>
#include <unordered_map>
#include <vector>
#include <typeindex>

class GameObjectManager
{
public:
    static GameObjectManager* GetInstance();

    static void Destroy();

    static GameObject* CreateGameObject(const std::string& objectName);

    template <typename Type>
    static Type* CreateGameObject();

    void Update();

    void Draw();

    void DrawUI();

    void Clear();

    void SetCamera(const Camera* camera) { camera_ = camera; }

    void SetGameObjectFactory(AbstractGameObjectFactory* gameObjectFactory) { gameObjectFactory_ = gameObjectFactory; }

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

    GameObject* CreateGameObjectInternal(const std::string& objectName);

    template <typename Type>
    Type* CreateGameObjectInternal();

    template <typename Type>
    Type* AcquireObjectFromPool();

    template <typename Type>
    void ReleaseObjectToPool(std::unique_ptr<Type> object);

    template <typename Type>
    Type* FindGameObject(const std::string& name, const std::vector<std::unique_ptr<GameObject>>& gameObjectList) const;

    template <typename Type>
    std::vector<Type*> FindGameObjects(const std::string& name, const std::vector<std::unique_ptr<GameObject>>& gameObjectList) const;

    void RemoveDestroyedObjects();

    void InitializeNewObjects();

private:
    static GameObjectManager* instance_;

    std::vector<std::unique_ptr<GameObject>> gameObjects_;

    std::vector<std::unique_ptr<GameObject>> newGameObjectsBuffer_;

    std::vector<std::unique_ptr<GameObject>> reusedObjectsBuffer_;

    std::unordered_map<std::type_index, std::queue<std::unique_ptr<GameObject>>> gameObjectPool_;

    const Camera* camera_ = nullptr;

    AbstractGameObjectFactory* gameObjectFactory_ = nullptr;
};

template<typename Type>
Type* GameObjectManager::CreateGameObject()
{
    //新しいゲームオブジェクトを内部で生成し、返す
    return GetInstance()->CreateGameObjectInternal<Type>();
}

template<typename Type>
Type* GameObjectManager::CreateGameObjectInternal()
{
    //オブジェクトプールからオブジェクトを取得
    Type* newObject = AcquireObjectFromPool<Type>();

    //プールにオブジェクトが存在しない場合、新規に生成
    if (!newObject)
    {
        newObject = new Type();
        newObject->AddComponent<TransformComponent>();
        newObject->SetGameObjectManager(this);
        newGameObjectsBuffer_.emplace_back(newObject);
    }
    else
    {
        //既存オブジェクトを再利用
        newObject->SetGameObjectManager(this);
        newObject->SetIsDestroy(false);
        reusedObjectsBuffer_.emplace_back(newObject);
    }
    return newObject;
}

template<typename Type>
Type* GameObjectManager::AcquireObjectFromPool()
{
    //オブジェクトプールから指定された型のオブジェクトを取得
    auto it = gameObjectPool_.find(std::type_index(typeid(Type)));
    if (it != gameObjectPool_.end() && !it->second.empty())
    {
        std::unique_ptr<GameObject> obj = std::move(it->second.front());
        it->second.pop();
        return static_cast<Type*>(obj.release());
    }
    return nullptr;
}

template<typename Type>
void GameObjectManager::ReleaseObjectToPool(std::unique_ptr<Type> object)
{
    //使用しなくなったオブジェクトをオブジェクトプールに戻す
    gameObjectPool_[std::type_index(typeid(*object))].emplace(std::move(object));
}

template <typename Type>
const Type* GameObjectManager::GetConstGameObject(const std::string& name) const
{
    //名前でゲームオブジェクトを検索 (const版)
    const Type* obj = FindGameObject<Type>(name, newGameObjectsBuffer_);
    if (obj) return obj;

    obj = FindGameObject<Type>(name, reusedObjectsBuffer_);
    if (obj) return obj;

    return FindGameObject<Type>(name, gameObjects_);
}

template <typename Type>
const std::vector<Type*> GameObjectManager::GetConstGameObjects(const std::string& name) const
{
    //名前で複数のゲームオブジェクトを検索 (const版)
    std::vector<Type*> result = FindGameObjects<Type>(name, newGameObjectsBuffer_);

    //プールから取得したゲームオブジェクトも追加
    const auto reusedObjects = FindGameObjects<Type>(name, reusedObjectsBuffer_);
    result.insert(result.end(), reusedObjects.begin(), reusedObjects.end());

    //既存のゲームオブジェクトも追加
    const auto mutableObjects = FindGameObjects<Type>(name, gameObjects_);
    result.insert(result.end(), mutableObjects.begin(), mutableObjects.end());

    return result;
}

template <typename Type>
Type* GameObjectManager::GetMutableGameObject(const std::string& name) const
{
    //名前でゲームオブジェクトを検索
    Type* obj = FindGameObject<Type>(name, newGameObjectsBuffer_);
    if (obj) return obj;

    obj = FindGameObject<Type>(name, reusedObjectsBuffer_);
    if (obj) return obj;

    return FindGameObject<Type>(name, gameObjects_);
}

template <typename Type>
std::vector<Type*> GameObjectManager::GetMutableGameObjects(const std::string& name) const
{
    //名前で複数のゲームオブジェクトを検索
    std::vector<Type*> result = FindGameObjects<Type>(name, newGameObjectsBuffer_);

    //プールから取得したゲームオブジェクトも追加
    const auto reusedObjects = FindGameObjects<Type>(name, reusedObjectsBuffer_);
    result.insert(result.end(), reusedObjects.begin(), reusedObjects.end());

    //既存のゲームオブジェクトも追加
    const auto mutableObjects = FindGameObjects<Type>(name, gameObjects_);
    result.insert(result.end(), mutableObjects.begin(), mutableObjects.end());

    return result;
}

template <typename Type>
Type* GameObjectManager::FindGameObject(const std::string& name, const std::vector<std::unique_ptr<GameObject>>& gameObjectList) const
{
    //指定されたリストから名前でゲームオブジェクトを検索
    for (const auto& gameObject : gameObjectList)
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
std::vector<Type*> GameObjectManager::FindGameObjects(const std::string& name, const std::vector<std::unique_ptr<GameObject>>& gameObjectList) const
{
    //指定されたリストから名前で複数のゲームオブジェクトを検索
    std::vector<Type*> gameObjects;
    for (const auto& gameObject : gameObjectList)
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
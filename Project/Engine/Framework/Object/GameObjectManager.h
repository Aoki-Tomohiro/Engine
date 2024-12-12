/**
 * @file GameObjectManager.h
 * @brief ゲームオブジェクトを管理するクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Framework/Object/AbstractGameObjectFactory.h"
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include <vector>

class GameObjectManager
{
public:
    /// <summary>
    /// インスタンスを取得
    /// </summary>
    /// <returns>インスタンス</returns>
    static GameObjectManager* GetInstance();

    /// <summary>
    /// 破棄処理
    /// </summary>
    static void Destroy();

    /// <summary>
    /// ゲームオブジェクトを生成（template）
    /// </summary>
    /// <typeparam name="Type">ゲームオブジェクトのタイプ</typeparam>
    /// <param name="objectName">オブジェクトの名前</param>
    /// <returns>生成ゲームオブジェクト</returns>
    template <typename Type>
    static Type* CreateGameObject(const std::string& objectName);

	/// <summary>
	/// ゲームオブジェクトを生成
	/// </summary>
	/// <typeparam name="Type">ゲームオブジェクトのタイプ</typeparam>
	/// <param name="objectName">オブジェクトの名前</param>
	/// <returns>生成ゲームオブジェクト</returns>
    static GameObject* CreateGameObject(const std::string& objectName);
    
	/// <summary>
	/// ゲームオブジェクトをクリア
	/// </summary>
	void Clear();

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera& camera);

	/// <summary>
	/// UIを描画
	/// </summary>
	void DrawUI();

	/// <summary>
	/// ゲームオブジェクトファクトリーを設定
	/// </summary>
	/// <param name="gameObjectFactory">ゲームオブジェクトファクトリー</param>
	void SetGameObjectFactory(AbstractGameObjectFactory* gameObjectFactory) { gameObjectFactory_ = gameObjectFactory; };

	/// <summary>
	/// ゲームオブジェクトを取得（template）
	/// </summary>
	/// <typeparam name="Type">ゲームオブジェクトの種類</typeparam>
	/// <param name="objectName">オブジェクトの名前</param>
	/// <returns>ゲームオブジェクト</returns>
	template <typename Type>
	Type* GetGameObject(const std::string& objectName) const;

	/// <summary>
	/// ゲームオブジェクトをまとめて取得
	/// </summary>
	/// <typeparam name="Type">ゲームオブジェクトの種類</typeparam>
	/// <param name="objectName">ゲームオブジェクトの名前</param>
	/// <returns>ゲームオブジェクトの配列</returns>
	template <typename Type>
	std::vector<Type*> GetGameObjects(const std::string& objectName) const;

private:
    GameObjectManager() = default;
    ~GameObjectManager() = default;
    GameObjectManager(const GameObjectManager&) = delete;
    const GameObjectManager& operator=(const GameObjectManager&) = delete;

    /// <summary>
    /// ゲームオブジェクトを内部で生成（template）
    /// </summary>
    /// <typeparam name="Type">ゲームオブジェクトの種類</typeparam>
    /// <param name="objectName">ゲームオブジェクトの名前</param>
    /// <returns>生成したゲームオブジェクト</returns>
    template <typename Type>
	Type* CreateGameObjectInternal(const std::string& objectName);

    /// <summary>
    /// ゲームオブジェクトを内部で生成
    /// </summary>
    /// <param name="objectName">ゲームオブジェクトの名前</param>
    /// <returns>生成したゲームオブジェクト</returns>
    GameObject* CreateGameObjectInternal(const std::string& objectName);

    /// <summary>
    /// ゲームオブジェクトを削除
    /// </summary>
    void RemoveDestroyedGameObjects();

    /// <summary>
    /// 待機中のゲームオブジェクトを初期化
    /// </summary>
    void ProcessPendingGameObjects();

	/// <summary>
	/// ゲームオブジェクトを名前ベースで見つける
	/// </summary>
	/// <typeparam name="Type">ゲームオブジェクトの種類</typeparam>
	/// <param name="objects">ゲームオブジェクトの配列</param>
	/// <param name="objectName">ゲームオブジェクトの名前</param>
	/// <returns>見つかったゲームオブジェクト</returns>
	template <typename Type>
	Type* FindGameObjectByName(const std::vector<std::unique_ptr<GameObject>>& objects, const std::string& objectName) const;

	/// <summary>
	/// 指定した名前のゲームオブジェクトを配列に追加
	/// </summary>
	/// <typeparam name="Type">ゲームオブジェクトの種類</typeparam>
	/// <param name="objects">ゲームオブジェクトの配列</param>
	/// <param name="objectName">ゲームオブジェクトの名前</param>
	/// <param name="result">追加するゲームオブジェクトの配列</param>
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
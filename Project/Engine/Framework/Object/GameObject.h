/**
 * @file GameObject.h
 * @brief ゲームオブジェクト
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Components/Base/Component.h"
#include "Engine/Components/Base/RenderComponent.h"
#include <list>
#include <memory>
#include <string>
#include <vector>

class GameObjectManager;

class GameObject
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~GameObject() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() {};

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	virtual void Draw(const Camera& camera);

	/// <summary>
	/// UIの描画
	/// </summary>
	virtual void DrawUI() {};

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="">衝突相手</param>
	virtual void OnCollision(GameObject*) {};

	/// <summary>
	/// 衝突判定（触れた瞬間）
	/// </summary>
	/// <param name="">衝突相手</param>
	virtual void OnCollisionEnter(GameObject*) {};

	/// <summary>
	/// 衝突判定（離れた瞬間）
	/// </summary>
	/// <param name=""></param>
	virtual void OnCollisionExit(GameObject*) {};

	//名前の取得・設定
	const std::string& GetName() const { return name_; };
	void SetName(const std::string& name) { name_ = name; };

	//アクティブ状態の取得・設定
	const bool GetIsActive() const { return isActive_; };
	void SetIsActive(bool isActive) { isActive_ = isActive; };

	//描画フラグの取得・設定
	const bool GetIsVisible() const { return isVisible_; };
	void SetIsVisible(bool isVisible) { isVisible_ = isVisible; };

	//UIの描画フラグの取得・設定
	const bool GetIsUiVisible() const { return isUiVisible_; };
	void SetIsUiVisible(const bool isUiVisible) { isUiVisible_ = isUiVisible; };

	//破壊フラグの取得・設定
	const bool GetIsDestroy() const { return isDestroy_; };
	void SetIsDestroy(bool isDestroy) { isDestroy_ = isDestroy; };

	//ゲームオブジェクトマネージャーを設定
	void SetGameObjectManager(GameObjectManager* gameObjectManager) { gameObjectManager_ = gameObjectManager; };

	//コンポーネントを追加
	template<class Type>
	Type* AddComponent();

	//コンポーネントを取得
	template<class Type>
	Type* GetComponent();

	//指定した全てのコンポーネントを取得
	template <typename Type>
	std::vector<Type*> GetComponents();

protected:
	//ゲームオブジェクトマネージャー
	GameObjectManager* gameObjectManager_ = nullptr;

	//コンポーネント
	std::list<std::unique_ptr<Component>> components_{};

	//名前
	std::string name_{};

	//描画フラグ
	bool isVisible_ = true;

	//UIの描画フラグ
	bool isUiVisible_ = true;

	//アクティブフラグ
	bool isActive_ = true;

	//破壊フラグ
	bool isDestroy_ = false;
};


template<class Type>
Type* GameObject::AddComponent()
{
	Type* component = new Type();
	component->Initialize();
	component->owner_ = this;
	components_.push_back(std::unique_ptr<Type>(component));
	return component;
}

template<class Type>
Type* GameObject::GetComponent()
{
	for (const auto& component : components_)
	{
		if (auto* specificComponent = dynamic_cast<Type*>(component.get()))
		{
			return specificComponent;
		}
	}
	return nullptr;
}

template <typename Type>
std::vector<Type*> GameObject::GetComponents()
{
	std::vector<Type*> result;
	for (const auto& component : components_)
	{
		if (auto* specificComponent = dynamic_cast<Type*>(component.get()))
		{
			result.push_back(specificComponent);
		}
	}
	return result;
}
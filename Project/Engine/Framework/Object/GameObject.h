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
	virtual ~GameObject() = default;

	virtual void Initialize() {};

	virtual void Update();

	virtual void Draw(const Camera& camera);

	virtual void DrawUI() {};

	virtual void OnCollision(GameObject*) {};

	virtual void OnCollisionEnter(GameObject*) {};

	virtual void OnCollisionExit(GameObject*) {};

	const std::string& GetName() const { return name_; };

	void SetName(const std::string& name) { name_ = name; };

	const bool GetIsActive() const { return isActive_; };

	void SetIsActive(bool isActive) { isActive_ = isActive; };

	const bool GetIsVisible() const { return isVisible_; };

	void SetIsVisible(bool isVisible) { isVisible_ = isVisible; };

	const bool GetIsUiVisible() const { return isUiVisible_; };

	void SetIsUiVisible(const bool isUiVisible) { isUiVisible_ = isUiVisible; };

	const bool GetIsDestroy() const { return isDestroy_; };

	void SetIsDestroy(bool isDestroy) { isDestroy_ = isDestroy; };

	void SetGameObjectManager(GameObjectManager* gameObjectManager) { gameObjectManager_ = gameObjectManager; };

	template<class Type>
	Type* AddComponent();

	template<class Type>
	Type* GetComponent();

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
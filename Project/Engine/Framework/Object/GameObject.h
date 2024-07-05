#pragma once
#include "Engine/Components/Component/Component.h"
#include "Engine/Components/Component/RenderComponent.h"
#include <list>
#include <memory>
#include <string>
#include <vector>

class GameObjectManager;

class GameObject
{
public:
	virtual ~GameObject() = default;

	virtual void Initialize();

	virtual void Update();

	virtual void Draw(const Camera& camera);

	virtual void DrawUI();

	virtual void OnCollision(GameObject* collider) = 0;

	virtual void OnCollisionEnter(GameObject* collider) = 0;

	virtual void OnCollisionExit(GameObject* collider) = 0;

	const std::string& GetTag() const { return tag_; };

	void SetTag(const std::string& tag) { tag_ = tag; };

	const bool GetIsVisible() const { return isVisible_; };

	void SetIsVisible(bool isVisible) { isVisible_ = isVisible; };

	const bool GetIsActive() const { return isActive_; };

	void SetIsActive(bool isActive) { isActive_ = isActive; };

	const bool GetIsDestroy() const { return isDestroy_; };

	void SetIsDestroy(bool isDestroy) { isDestroy_ = isDestroy; };

	void SetGameObjectManager(const GameObjectManager* gameObjectManager) { gameObjectManager_ = gameObjectManager; };

	template<class Type>
	Type* GetComponent();

	template <typename Type>
	std::vector<Type*> GetComponents();

	template<class Type>
	Type* AddComponent();

private:
	//ゲームオブジェクトマネージャー
	const GameObjectManager* gameObjectManager_ = nullptr;

	//コンポーネント
	std::list<std::unique_ptr<Component>> components_{};

	//タグ
	std::string tag_{};

	//描画フラグ
	bool isVisible_ = true;

	//アクティブフラグ
	bool isActive_ = true;

	//破壊フラグ
	bool isDestroy_ = false;
};

template<class Type>
inline Type* GameObject::GetComponent()
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

template<class Type>
inline Type* GameObject::AddComponent()
{
	Type* component = new Type();
	component->owner_ = this;
	components_.push_back(std::unique_ptr<Type>(component));
	return component;
}
#pragma once
#include "Engine/3D/Model/Model.h"

class GameObjectManager;

class IGameObject
{
public:
	virtual ~IGameObject() = default;

	virtual void Initialize();

	virtual void Update();

	virtual void Draw(const Camera& camera);

	virtual void DrawUI();

	Model* GetModel() const { return model_; };

	void SetModel(Model* model) { model_ = model; };

	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; };

	void SetRotation(const Vector3& rotation) { worldTransform_.rotation_ = rotation; };

	void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; };

	const std::string& GetTag() const { return tag_; };

	void SetTag(const std::string& tag) { tag_ = tag; };

	const bool GetIsVisible() const { return isVisible_; };

	void SetIsVisible(bool isVisible) { isVisible_ = isVisible; };

	const bool GetIsActive() const { return isActive_; };

	void SetIsActive(bool isActive) { isActive_ = isActive; };

	const bool GetIsDestroy() const { return isDestroy_; };

	void SetIsDestroy(bool isDestroy) { isDestroy_ = isDestroy; };

	void SetGameObjectManager(const GameObjectManager* gameObjectManager) { gameObjectManager_ = gameObjectManager; };

protected:
	//ゲームオブジェクトマネージャー
	const GameObjectManager* gameObjectManager_ = nullptr;

	//ワールドトランスフォーム
	WorldTransform worldTransform_{};

	//モデル
	Model* model_ = nullptr;

	//タグ
	std::string tag_{};

	//描画フラグ
	bool isVisible_ = true;

	//アクティブフラグ
	bool isActive_ = true;

	//破壊フラグ
	bool isDestroy_ = false;
};


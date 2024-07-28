#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"

class Ground : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

private:
	//色
	Vector4 color_ = { 0.118f,0.118f,0.118f,1.0f };

	//鏡面反射の色
	Vector3 specularColor_ = { 0.0f,0.0f,0.0f };
};


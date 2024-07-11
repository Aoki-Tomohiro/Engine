#pragma once
#include "Engine/Framework/Object/GameObject.h"

class Illusion : public GameObject
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
	Vector4 color_{ 1.0f,1.0f,1.0f,1.0f };
};


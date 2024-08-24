#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"

class Player : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* other) override;

	void OnCollisionEnter(GameObject* other) override;

	void OnCollisionExit(GameObject* other) override;

private:

};


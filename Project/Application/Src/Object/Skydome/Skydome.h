#pragma once
#include "Engine/Framework/Object/GameObject.h"

class Skydome : public GameObject
{
public:
	void Initialize() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

private:
};


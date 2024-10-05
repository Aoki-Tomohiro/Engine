#pragma once
#include "Engine/Framework/Object/GameObject.h"

class BackGroundObject : public GameObject
{
public:
	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

private:
};


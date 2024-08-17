#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Math/MathFunction.h"

class Enemy : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	const Vector3 GetHipWorldPosition() const;

private:
	//ModelComponent
	ModelComponent* modelComponent_ = nullptr;

	//Quaternion
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	//回転の補間速度
	float quaternionInterpolationSpeed_ = 0.4f;

	//TimeScale
	float timeScale_ = 1.0f;
};


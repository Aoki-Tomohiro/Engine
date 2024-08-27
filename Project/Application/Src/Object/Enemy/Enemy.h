#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"
#include <numbers>

class Enemy : public GameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	const Vector3 GetHipWorldPosition();

	void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

private:
	void UpdateRotation();

private:
	//Quaternion
	Quaternion destinationQuaternion_ = Mathf::IdentityQuaternion();

	//回転の補間速度
	float quaternionInterpolationSpeed_ = 0.4f;

	//タイトルシーンなのか
	bool isInTitleScene_ = false;
};


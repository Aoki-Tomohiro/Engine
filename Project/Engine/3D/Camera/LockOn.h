#pragma once
#include "Camera.h"
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/2D/Sprite.h"

class LockOn
{
public:
	void Initialize();

	void Update(GameObject* gameObject, const Camera* camera);

	void Draw();

	Vector3 GetTargetPosition() const;

	bool ExistTarget() const { return target_ ? true : false; };

	void SetLockOnMark(const std::string& name) { lockOnMark_->SetTexture(name); };

private:
	bool InRange(const Camera* camera);

	void SearchLockOnTarget(GameObject* gameObject, const Camera* camera);

	Vector2 WorldToScreenPosition(const Vector3& worldPosition, const Camera* camera);

	void SetLockOnMarkPosition(const Camera* camera);

private:
	Input* input_ = nullptr;

	std::unique_ptr<Sprite> lockOnMark_ = nullptr;

	GameObject* target_ = nullptr;

	float minDistance_ = 10.0f;

	float maxDistance_ = 80.0f;

	const float kDegreeToRadian = 45.0f * 3.141592654f / 180.0f;

	float angleRange_ = 20.0f * kDegreeToRadian;
};


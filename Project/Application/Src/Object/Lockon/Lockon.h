#pragma once
#include "Engine/2D/Sprite.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Base/Application.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

class Lockon
{
public:
	void Initialize();

	void Update(const Camera* camera);

	void Draw();

	const Vector3 GetTargetPosition() const;

	bool ExistTarget() const { return target_ ? true : false; };

private:
	void UpdateTargeting();

	Vector2 WorldToScreenPosition(const Vector3& worldPosition, const Camera* camera);

	void SetLockonMarkPosition(const Camera* camera);

private:
	Input* input_ = nullptr;

	std::unique_ptr<Sprite> lockonMark_ = nullptr;

	Enemy* target_ = nullptr;
};


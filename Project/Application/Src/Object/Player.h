#pragma once
#include "Engine/Framework/Object/IGameObject.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"

class Player : public IGameObject
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;
	
	const WorldTransform& GetWorldTransform() const { return worldTransform_; };

private:
	Input* input_ = nullptr;

	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };
};


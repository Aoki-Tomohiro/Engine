#pragma once
#include "IPlayerState.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Utilities/GlobalVariables.h"

class PlayerStateJustDodge : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void OnCollision(GameObject* other) override;

	void OnCollisionEnter(GameObject* other) override;

	void OnCollisionExit(GameObject* other) override;

private:
	void ApplyGlobalVariables();

private:
	Input* input_ = nullptr;

	Vector3 offset_ = { 0.0f,0.0f,-10.0f };

	float distance_ = 10.0f;
};


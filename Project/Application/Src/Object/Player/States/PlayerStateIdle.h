#pragma once
#include "IPlayerState.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Component/TransformComponent.h"

class PlayerStateRoot : public IPlayerState
{
public:
	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

private:
	void AppliGlobalVariables();

private:
	Input* input_ = nullptr;

	float moveSpeed_ = 0.4f;


};


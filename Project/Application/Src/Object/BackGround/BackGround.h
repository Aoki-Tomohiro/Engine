#pragma once
#include "Engine/3D/Skybox/Skybox.h"

class BackGround
{
public:
	void Initialize(Skybox* skybox);

	void Update();

	void Draw(const Camera& camera);

private:
	Skybox* skybox_ = nullptr;

	WorldTransform worldTransform_{};
};


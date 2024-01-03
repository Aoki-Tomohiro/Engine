#pragma once
#include "Engine/Framework/GameCore.h"
#include "Project/Scene/SceneFactory.h"

class GameManager : public GameCore
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

private:

};


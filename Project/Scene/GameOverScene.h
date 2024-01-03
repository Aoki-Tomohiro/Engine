#pragma once
#include "Engine/Framework/IScene.h"

class GameOverScene : public IScene 
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

private:

};


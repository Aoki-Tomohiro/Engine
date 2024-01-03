#pragma once
#include "Engine/Framework/IScene.h"
#include "Engine/Components/Input.h"

class GameClearScene : public IScene
{
public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

private:
	Input* input_ = nullptr;
};


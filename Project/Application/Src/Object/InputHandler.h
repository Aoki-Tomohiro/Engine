#pragma once
#include "MoveLeftCommand.h"
#include "MoveRightCommand.h"
#include "Engine/Components/Input/Input.h"

class InputHandler {
public:
	InputHandler();
	~InputHandler();
	ICommand* HandleInput();
	void AssingMoveLeftCommand2PressKeyA();
	void AssingMoveRightCommand2PressKeyD();

private:
	Input* input_ = nullptr;
	ICommand* pressKeyA_ = nullptr;
	ICommand* pressKeyD_ = nullptr;
};


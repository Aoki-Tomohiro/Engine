#include "InputHandler.h"

InputHandler::InputHandler() {
	input_ = Input::GetInstance();
}

InputHandler::~InputHandler() {
	delete pressKeyA_;
	delete pressKeyD_;
}

void InputHandler::AssingMoveLeftCommand2PressKeyA() {
	pressKeyA_ = new MoveLeftCommand();
	this->pressKeyA_ = pressKeyA_;
}

void InputHandler::AssingMoveRightCommand2PressKeyD() {
	pressKeyD_ = new MoveRightCommand();
	this->pressKeyD_ = pressKeyD_;
}

ICommand* InputHandler::HandleInput() {
	if (input_->IsPushKey(DIK_A)) {
		return pressKeyA_;
	}
	if (input_->IsPushKey(DIK_D)) {
		return pressKeyD_;
	}
	return nullptr;
}
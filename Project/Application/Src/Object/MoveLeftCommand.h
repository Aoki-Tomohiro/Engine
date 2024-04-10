#pragma once
#include "ICommand.h"

class MoveLeftCommand : public ICommand {
public:
	~MoveLeftCommand() override = default;
	void Exec(Player& player) override;
};


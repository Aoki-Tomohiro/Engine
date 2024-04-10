#pragma once
#include "ICommand.h"

class MoveRightCommand : public ICommand {
public:
	~MoveRightCommand() override = default;
	void Exec(Player& player) override;
};


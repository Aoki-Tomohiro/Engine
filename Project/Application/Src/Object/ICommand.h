#pragma once

class Player;

class ICommand {
public:
	virtual ~ICommand() = default;
	virtual void Exec(Player& player) = 0;
};

#include "W4Strategy.h"
#include "MDebug.h"
#include <iostream>

std::string W4Strategy::getName() const {
	return "";
}

W4Strategy::~W4Strategy() {
	// nothing to do
}

//
// W4HumanPlayerStrategy
//

int W4HumanPlayerStrategy::nextMove(W4Board_p board) {
	int column = -1;
	for (;;) {
		std::cin >> column;
		if (column < 1 || column > NUM_COLUMNS) {
			std::cout << "Dieser Wert ist ungültig, bitte versuche es nochmal: ";
		}
		else if (board->colorOf(NUM_ROWS - 1, column - 1) != COLOR_NONE) {
			std::cout << "Diese Spalte ist voll, bitte versuche es nochmal: ";
		}
		else {
			return column - 1;
		}
	}
}

std::string W4HumanPlayerStrategy::getName() const {
	return _name;
}

void W4HumanPlayerStrategy::setName(const std::string& name) {
	_name = name;
}

std::string W4HumanPlayerStrategy::getPlayerType() const {
	return "human";
}

W4HumanPlayerStrategy::~W4HumanPlayerStrategy() {
	// nothing to do
}

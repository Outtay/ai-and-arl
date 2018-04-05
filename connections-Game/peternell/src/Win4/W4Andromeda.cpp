
#include "W4Andromeda.h"
#include "MDebug.h"
#include <iostream>
#include <list>


W4Heuristic::~W4Heuristic() {
	// Nothing to do
}

int W4HeuristicCenter::calculateValue(W4Board_p board) {
	int result = 0;
	int colValues[] = { 0,1,2,4,2,1,0 };
	for (int col = 0; col < NUM_COLUMNS; col++) {
		int colValue = colValues[col];
		for (int row = 0; row < NUM_ROWS; row++) {
			W4Color color = board->colorOf(row, col, false);
			if (color == COLOR_BLACK) {
				result += colValue;
			}
			else if (color == COLOR_RED) {
				result -= colValue;
			}
			else {
				break;
			}
		}
	}
	return result;
}

W4HeuristicCenter::~W4HeuristicCenter() {
	// Nothing to do
}

int W4SmartHeuristic::calculateValue(W4Board_p board) {
	const int BUF_LEN = NUM_ROWS * NUM_COLUMNS;
	const char blackBit = 1;
	const char redBit = 2;
	char heatMap[BUF_LEN];

	int blackChances = 0;
	int redChances = 0;

	int idx = 0;
	for (int row = 0; row < NUM_ROWS; row++) {
		for (int col = 0; col < NUM_COLUMNS; col++) {
			bool blackWins = board->wouldWinAt(row, col, COLOR_BLACK);
			bool redWins = board->wouldWinAt(row, col, COLOR_RED);

			heatMap[idx] = (blackWins ? blackBit : 0) | (redWins ? redBit : 0);
			idx++;
		}
	}

	for (int col = 0; col < NUM_COLUMNS; col++) {
		int emptyCells = 0;

		for (int row = 0; row < NUM_ROWS - 1; row++) {
			int idx1 = row*NUM_COLUMNS + col;
			int idx2 = idx1 + NUM_COLUMNS;
			
			char val1 = heatMap[idx1];
			char val2 = heatMap[idx2];

			bool smart = false;

			if (board->colorOf(row, col) == COLOR_NONE) {
				emptyCells++;
			}

			if (val1&blackBit) {
				blackChances++;
			}
			if (val1&redBit) {
				redChances++;
			}

			if ((val1&blackBit) && (val2&blackBit)) {
				blackChances += 15 - emptyCells;
				smart = true;
			}
			if ((val1&redBit) && (val2&redBit)) {
				redChances += 15 - emptyCells;
				smart = true;
			}
			if (smart) {
				break;
			}
		}
	}

	return (blackChances - redChances) * 10;
}

W4SmartHeuristic::~W4SmartHeuristic() {
	// Nothing to do
}

int W4Andromeda::nextMove(W4Board_p board)
{
	int bestColumn = INVALID_COLUMN;
	int bestValue = 9000;
	int alpha = -9000;
	int beta = +9000;
	for (int column = 0; column < NUM_COLUMNS; column++) {
		MDebug("{ %s: column %d (beta=%d)", W4ColorToCstr(board->getColor()), column + 1, beta);
		W4Board_p nextBoard = board->next(column);
		if (nextBoard != NULL) {
			int value = evalBoard(nextBoard, alpha, beta, m_optionDepth - 1);
			int randomEinfluss = 0;
			if (m_rng != NULL && m_optionRandomness > 0) {
				randomEinfluss += m_rng->randomInt(-m_optionRandomness, +m_optionRandomness);
			}
			MDebug("} %-9s: column %d * VALUE %5d%+d", W4ColorToCstr(board->getColor()), column + 1, value, randomEinfluss);
			value += randomEinfluss;
			if (value < bestValue) {
				bestValue = value;
				bestColumn = column;
			}
		}
		else {
			MDebug("} %-9s: column %d * Unentschieden", W4ColorToCstr(board->getColor()), column + 1);
		}
	}
	return bestColumn;
}

int W4Andromeda::evalBoard(W4Board_p board, int alpha, int beta, int maxDepth) {
	// Szenario:
	// board->getColor() = BLACK
	//
	// Fall 1: board->hasWon() == true
	//  => rot hat gewonnen
	//  return -5000
	// Fall 2: board->hasWon() == false, Board ist für rot sehr günstig, maxDepth == 0
	//  => heuristik soll kleinen Wert zurückgeben
	//  => heuristik-funktion gibt kleinen Wert zurück wenn rot besser da steht
	//  => multipliziere heuristik-funktion mit 1 == board->getColor() => passt.

	if (board->hasWon()) {
		return -5000;
	}
	else if (board->isFull()) {
		return 0;
	}
	if (maxDepth <= 0) {
		int hValue = m_heuristic->calculateValue(board);
		return hValue * board->getColor();
	}

	// Which options do we have?
	std::list<W4Board_p> ourOptions;
	for (int column = 0; column < NUM_COLUMNS; column++) {
		W4Board_p nextBoard = board->next(column);
		if (nextBoard == NULL) {
			continue;
		}
		if (nextBoard->hasWon()) {
			return 5000;
			// ^^this is an early exit point which speeds up the algorithm by 17%
		}
		ourOptions.push_back(nextBoard);
	}
	if (ourOptions.size() == 0) {
		// Das Brett ist voll.
		return 0;
	}

	// Recurse
	int maxValue = -5000;
	int sumOfAll = 0;
	for (W4Board_p nextBoard : ourOptions) {
		int nextValue = -evalBoard(nextBoard, -beta, -alpha, maxDepth - 1);
		if (nextValue < m_optionDilutionThreshold) {
			sumOfAll += nextValue;
		}
		//MDebug("  } %s: column %d * VALUE %4d", W4ColorToCstr(board->getColor()), column + 1, nextValue);
		if (nextValue > maxValue) {
			maxValue = nextValue;
		}
		if (m_optionEnableAlphaBetaPruning) {
			if (nextValue >= beta) {
				break;
			}
			if (nextValue > alpha) {
				alpha = nextValue;
			}
		}
	}

	if (m_optionAttenuationPercent > 0 || m_optionDilutionPercent > 0) {
		int result = maxValue;
		if (m_optionAttenuationPercent > 0) {
			result = result * (100 - m_optionAttenuationPercent) / 100;
		}
		if (m_optionDilutionPercent > 0) {
			result += sumOfAll * m_optionDilutionPercent / 100;
		}
	}

	return maxValue;
}

W4Andromeda::~W4Andromeda()
{
	// Nothing to do
}


#include "W4Board.h"
#include "MDebug.h"
#include <sstream>
#include <iostream>
#include <windows.h>

//
// Instrumentation
//

static uint64_t IAllocationCounter = 0;
static uint64_t IDeallocationCounter = 0;
static int IMaxInstanceCount = 0;


uint64_t W4Board::instanceAllocationCounter()
{
	return IAllocationCounter;
}

uint64_t W4Board::instanceDeallocationCounter()
{
	return IDeallocationCounter;
}

int W4Board::liveInstanceCounter()
{
	return int(IAllocationCounter - IDeallocationCounter);
}

int W4Board::liveMaxInstanceCounter()
{
	return IMaxInstanceCount;
}

void W4Board::resetLiveMaxInstanceCounter()
{
	IMaxInstanceCount = liveInstanceCounter();
}


//
// Instance methods
//

const char *W4ColorToCstr(W4Color c) {
	switch (c) {
	case COLOR_BLACK: return "Black('o')";
	case COLOR_RED: return "Red('X')";
	default: return "None";
	}
}

W4Board::W4Board() {
	IAllocationCounter++;

	int lic = liveInstanceCounter();
	if (lic > IMaxInstanceCount) {
		IMaxInstanceCount = lic;
	}
}

std::shared_ptr<W4Board> W4Board::emptyBoard() {
	std::shared_ptr<W4Board> board = std::shared_ptr<W4Board>(new W4Board());
	board->m_color = COLOR_BLACK;
	memset(&(board->m_fields), 0, sizeof(board->m_fields));
	board->m_won = false;
	return board;
}

static int fieldIndex(int row, int col) {
	if (row < 0 || row >= NUM_ROWS) {
		return -1;
	}
	if (col < 0 || col >= NUM_COLUMNS) {
		return -1;
	}
	return row*NUM_COLUMNS + col;
}

W4Color W4Board::colorOf(int row, int col, bool throwOnError) const {
	int fIndex = fieldIndex(row, col);
	if (fIndex == -1) {
		if (throwOnError) {
			throw new std::invalid_argument("argument row or col out of bounds");
		}
		else {
			return COLOR_NONE;
		}
	}

	return m_fields[fIndex];
}

bool W4Board::wouldWinAt(int row, int column, W4Color player) const {
	int fIdx = fieldIndex(row, column);
	if (fIdx == -1) {
		return false;
	}
	if (m_fields[fIdx] != 0) {
		return false;
	}
	if (player != COLOR_BLACK && player != COLOR_RED) {
		return false;
	}

	W4Board nextBoard;
	memcpy(nextBoard.m_fields, m_fields, sizeof(m_fields));
	nextBoard.m_fields[fIdx] = player;
	nextBoard.m_color = -player;
	bool hasWon = nextBoard.calculateHasWon(row, column);
	// 'nextBoard' is an invalid object! don't return or use it in any other way.
	return hasWon;
}

std::shared_ptr<W4Board> W4Board::next(int col) const {
	if (m_won) {
		MDebug("Warning: Trying to generate next board from board that already won!");
		return nullptr;
	}

	if (col < 0 || col >= NUM_COLUMNS) {
		throw std::invalid_argument("Parameter 'col' out of bounds");
	}

	int row = 0;
	for (;;) {
		W4Color fieldColor = colorOf(row, col, false);
		if (fieldColor == COLOR_NONE) {
			break;
		}
		row++;
	}

	if (row >= NUM_ROWS) {
		// In dieses Feld passt nix mehr
		return nullptr;
	}

	std::shared_ptr<W4Board> nextBoard = std::shared_ptr<W4Board>(new W4Board());
	nextBoard->m_color = -(this->m_color);
	memcpy(nextBoard->m_fields, this->m_fields, sizeof(this->m_fields));
	nextBoard->m_fields[fieldIndex(row, col)] = this->m_color;
	nextBoard->m_moveCount = this->m_moveCount + 1;
	nextBoard->m_lastSetColumn = col;
	nextBoard->m_lastSetRow = row;
	nextBoard->m_won = nextBoard->calculateHasWon(row, col);

	return nextBoard;
}

bool W4Board::calculateHasWon(int row, int col) {
	bool won = false;
	// Vertical?
	if (calculateHasWonDelta(row, col, 1, 0)) {
		//MDebug("User %s wins vertically", winnerStr());
		won = true;
	}

	// Horizontal
	if (calculateHasWonDelta(row, col, 0, 1)) {
		//MDebug("User %s wins horizontally", winnerStr());
		won = true;
	}

	// Diagnoal aufsteigend
	if (calculateHasWonDelta(row, col, 1, 1)) {
		//MDebug("User %s wins diagnoally ascending", winnerStr());
		won = true;
	}

	// Diagnoal absteigend
	if (calculateHasWonDelta(row, col, -1, 1)) {
		//MDebug("User %s wins diagnoally descending", winnerStr());
		won = true;
	}

	if (won) {
		m_winningFields.emplace_back(row, col);
	}
	return won;
}

bool W4Board::calculateHasWonDelta(int row, int col, int rowDelta, int colDelta) {
	char winnerColor = -(this->m_color);
	int rowTmp = row + rowDelta;
	int colTmp = col + colDelta;
	int count = 1;
	std::vector<std::pair<int, int>> winningFields;
	while (colorOf(rowTmp, colTmp, false) == winnerColor) {
		count++;
		winningFields.emplace_back(rowTmp, colTmp);
		rowTmp += rowDelta;
		colTmp += colDelta;
	}
	rowTmp = row - rowDelta;
	colTmp = col - colDelta;
	while (colorOf(rowTmp, colTmp, false) == winnerColor) {
		count++;
		winningFields.emplace_back(rowTmp, colTmp);
		rowTmp -= rowDelta;
		colTmp -= colDelta;
	}
	if (count >= 4) {
		for (auto p : winningFields) {
			m_winningFields.push_back(p);
		}
	}
	return count >= 4;
}

void W4Board::printBoard() const {
	using std::cout;
	using std::string;

	HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	int ordinaryColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	int redColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_RED;
	int redHighlightColor = redColor | BACKGROUND_INTENSITY;
	int blackColor = ordinaryColor;
	int blackHighlightColor = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	int backgroundColor = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
	int footerHighlightColor = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;

	string header;

	// Header
	for (int column = 0; column < NUM_COLUMNS; column++) {
		header += "+---";
	}

	header += "+\n";
	SetConsoleTextAttribute(outputHandle, backgroundColor);
	cout << header;

	// Contents
	int topRowFull = (1 << NUM_COLUMNS) - 1;
	for (int row = NUM_ROWS - 1; row >= 0; row--) {
		string line;
		for (int column = 0; column < NUM_COLUMNS; column++) {
			W4Color color = colorOf(row, column);
			auto wfBegin = m_winningFields.begin();
			auto wfEnd = m_winningFields.end();
			bool isHighlighted = (row == m_lastSetRow && column == m_lastSetColumn) || std::find(wfBegin, wfEnd, rc_pair(row,column)) != wfEnd;
			switch (color) {
			case COLOR_BLACK: 
				cout << "|";
				SetConsoleTextAttribute(outputHandle, isHighlighted ? blackHighlightColor : blackColor);
				cout << " o ";
				SetConsoleTextAttribute(outputHandle, backgroundColor);
				break;
			case COLOR_RED:
				cout << "|";
				SetConsoleTextAttribute(outputHandle, isHighlighted ? redHighlightColor : redColor);
				cout << " X ";
				SetConsoleTextAttribute(outputHandle, backgroundColor);

				break;
			default: 
				cout << "|   ";
				if (row == NUM_ROWS - 1) {
					topRowFull &= ~(1 << column);
				}
				break;
			}
		}
		cout << "|\n";
		cout << header;
	}

	// Footer
	string footer;
	for (int column = 0; column < NUM_COLUMNS; column++) {
		bool isFull = (topRowFull & (1 << column)) != 0;
		bool nextIsFull = (topRowFull & (1 << (column + 1))) != 0;
		cout << "|";
		if (!isFull) {
			SetConsoleTextAttribute(outputHandle, footerHighlightColor);
		}
		cout << " " + std::to_string(column + 1) + " ";
		if (!isFull && (nextIsFull || column == NUM_COLUMNS - 1)) {
			SetConsoleTextAttribute(outputHandle, backgroundColor);
		}
	}
	cout << "|\n";
	for (int column = 0; column < NUM_COLUMNS; column++) {
		bool isFull = (topRowFull & (1 << column)) != 0;
		bool nextIsFull = (topRowFull & (1 << (column+1))) != 0;
		cout << "+";
		if (!isFull) {
			SetConsoleTextAttribute(outputHandle, footerHighlightColor);
		}
		cout << "---";
		if (!isFull && (nextIsFull || column == NUM_COLUMNS - 1)) {
			SetConsoleTextAttribute(outputHandle, backgroundColor);
		}
	}
	cout << "+\n";
	SetConsoleTextAttribute(outputHandle, ordinaryColor);
}

W4Board::~W4Board() {
	IDeallocationCounter++;
}


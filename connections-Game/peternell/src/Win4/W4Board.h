#pragma once

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

/// number of columns
#define NUM_COLUMNS 7
#define INVALID_COLUMN (-1)

#define NUM_ROWS 6

#define COLOR_NONE 0
// Black maximizes, black starts. ASCII: "o"
#define COLOR_BLACK 1
// Red minimizes. ASCII: "X"
#define COLOR_RED (-1)

typedef signed char W4Color;


const char *W4ColorToCstr(W4Color c);

class W4Board : std::enable_shared_from_this<W4Board> {
public:
	static std::shared_ptr<W4Board> emptyBoard();

	// Farbe des Spielers, der jetzt an der Reihe ist (=Black für das empty board) ((bzw. an der Reihe wäre, falls gerade einer gewonnen hat))
	inline W4Color getColor() const { return m_color; };
	inline const char *getColorStr() const { return W4ColorToCstr(m_color); };
	inline bool hasWon() const { return m_won; }
	inline W4Color whoHasWon() const { return m_won ? -m_color : COLOR_NONE; };
	inline bool isFull() const { return (m_moveCount == NUM_COLUMNS*NUM_ROWS); };
	inline int lastSetRow() const { return m_lastSetRow; };
	inline int lastSetColumn() const { return m_lastSetColumn; };
	W4Color colorOf(int row, int col, bool throwOnError = true) const;

	// Returns true if the 'player' would win if he could place his coin into (row,column).
	// The position (row,column) doesn't have to be available, but it has to be empty.
	bool wouldWinAt(int row, int column, W4Color player) const;

	// Generiert ein neues Board, das dadurch entsteht, dass der aktuelle Spieler (this->color) einen Stein in die angegebene Spalte wirft
	// @param col Die Spalte, in die geworfen werden soll. Muss zwischen 0-6 liegen, anderenfalls wird eine std::invalid_argument exception geworfen
	// @return Ein neues Board, in dem an der entsprechenden Stelle ein Stein eingefügt wurde. Der Spieler wurde gewechselt (see W4ColorOther).
	//         Oder nullptr, wenn an dieser Stelle kein Stein eingefügt werden kann, weil die Spalte voll ist.
	std::shared_ptr<W4Board> next(int col) const;

	void printBoard() const;

	virtual ~W4Board();

	// Instrumentation:

	static uint64_t instanceAllocationCounter();
	static uint64_t instanceDeallocationCounter();
	static int liveInstanceCounter();
	static int liveMaxInstanceCounter();
	static void resetLiveMaxInstanceCounter();

private:
	W4Board();

	bool calculateHasWon(int row, int col);
	bool calculateHasWonDelta(int row, int col, int rowDelta, int colDelta);

	inline const char *winnerStr() const { return W4ColorToCstr(-m_color); };

	W4Color m_fields[NUM_COLUMNS*NUM_ROWS];
	// Row/column pair
	typedef std::pair<int, int> rc_pair;
	std::vector<rc_pair> m_winningFields;
	short m_moveCount = 0;
	char m_lastSetColumn = -1;
	char m_lastSetRow = -1;
	W4Color m_color;
	bool m_won;
};

typedef std::shared_ptr<W4Board> W4Board_p;


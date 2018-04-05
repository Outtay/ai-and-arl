#pragma once

#include "W4Board.h"
#include <string>
#include <memory>
#include <stdexcept>


// Eine Spielstrategie. Wichtigste Funktion ist 'nextMove', das für jedes Board einen Move berechnen soll.
class W4Strategy
{
public:
	// In welche Spalte soll gesetzt werden? Return -1 im Fehlerfall (sollte aber nicht auftreten, außer das Board ist voll.)
	virtual int nextMove(W4Board_p board) = 0;

	// Player name (defaults to "")
	virtual std::string getName() const;

	// Art des Spielers:
	// - "human" für einen menschlichen Spieler. (D.h. nextMove() muss den Benutzer fragen.)
	// - anderes für KI-Gegner
	virtual std::string getPlayerType() const = 0;

	virtual ~W4Strategy();
};

typedef std::shared_ptr<W4Strategy> W4Strategy_p;

class W4HumanPlayerStrategy : public W4Strategy
{
public:
	int nextMove(W4Board_p board);

	std::string getName() const;
	void setName(const std::string& name);

	// Returns "human"
	std::string getPlayerType() const;

	~W4HumanPlayerStrategy();

private:
	std::string _name;
};

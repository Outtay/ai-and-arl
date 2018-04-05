#pragma once

#include "W4Strategy.h"
#include "W4Random.h"
#include <memory>

class W4Heuristic
{
public:
	virtual int calculateValue(W4Board_p board) = 0;
	virtual ~W4Heuristic();
};

class W4HeuristicCenter : public W4Heuristic
{
public:
	int calculateValue(W4Board_p board);
	~W4HeuristicCenter();
};

class W4SmartHeuristic : public W4Heuristic
{
public:
	int calculateValue(W4Board_p board);
	~W4SmartHeuristic();
};

class W4Andromeda : public W4Strategy
{
public:
	int nextMove(W4Board_p board);
	std::string getName() const { return m_name; };
	void setName(const std::string& name) { m_name = name; };

	std::string getPlayerType() const { return "andromeda"; };

	~W4Andromeda();

	// Konfiguration...

	int m_optionDepth = 5;
	int m_optionRandomness = 5;
	int m_optionAttenuationPercent = 10;
	int m_optionDilutionThreshold = 0;
	int m_optionDilutionPercent = 6;
	bool m_optionEnableAlphaBetaPruning = true;
	std::shared_ptr<W4Heuristic> m_heuristic;
	std::shared_ptr<W4Random> m_rng;

private:
	// Größerer Wert heißt: Board ist gut für den Spieler, der an der Reihe ist.
	int evalBoard(W4Board_p board, int alpha, int beta, int maxDepth);

	std::string m_name;
};

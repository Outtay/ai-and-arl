// main.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "MDebug.h"
#include "W4Board.h"
#include "W4Strategy.h"
#include "W4Random.h"
#include "W4Andromeda.h"
#include <assert.h>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <chrono>

using namespace std;

bool _MDebug_Enabled = false;


static void usage() {
	printf("Usage: Win4.exe [options] <PLAYER-1-SPEC(black)> <PLAYER-2-SPEC(red)>\n");
	printf("Options:\n");
	printf("  /seed=123 . . . . . Specify random seed (between 1 and 2^63 - 1)\n");
	printf("  /seed=0 . . . . . . Initialize random seed by current time (DEFAULT)\n");
	printf("  /no-ab  . . . . . . Disable alpha-beta pruning\n");
	printf("  /test . . . . . . . Lasse Spieler 100 Mal spielen => Statistik\n");
	printf("  /help . . . . . . . Print this information\n");
	printf("  /verbose  . . . . . Print debug output\n");
	printf("Player specifications:\n");
	printf("  human . . . . . . . Menschlicher Spieler\n");
	printf("  human:Max . . . . . Menschlicher Spieler mit Namen\n");
	printf("  ai-42 . . . . . . . KI mit unbeschraenkter Suchtiefe\n");
	printf("  ai-5  . . . . . . . KI mit bestimmter Suchtiefe (hier: 5. Gueltig: 2 bis 10)\n");
	printf("  ci-NUMBER . . . . . Wie \"ai-*\", aber mit schlechterer \"center\"-Heuristik\n");
	printf("  pi-NUMBER . . . . . Wie \"ai-*\", aber ohne Extra-Tricks (attenuation,dilution,random)\n");
#if defined(_DEBUG) || defined(DEBUG)
	printf("* DEBUG VERSION *\n");
#else
	printf("* RELEASE VERSION *\n");
#endif
}

static bool strStartsWith(const std::string& base, const std::string& search)
{
	int size1 = search.size();
	int size2 = base.size();
	int size = (size1 < size2) ? size1 : size2;

	// Cannot use std::min in Visual C++, because min() and max() are macros :-o

	return std::equal(
		search.begin(),
		search.begin() + size,
		base.begin());
}

// A class to calculate min, max, avg, stddev of any sequence of values.
// Unlike Knuths algorithm, this is a bit susceptible for floating point rounding errors,
// but for our purposes it is more than good enough.
class StatVar
{
private:
	int m_count = 0;
	double m_min = 0.0;
	double m_max = 0.0;
	double m_sum = 0.0; // E(X) = m_sum / m_count
	double m_squareSum = 0.0; // E(X^2) = m_squareSum / m_count

	static inline double sqr(double x) { return x*x; }

	inline double getAvgOfSqr() const { return m_squareSum / m_count; }

	// Convert double to string with 'n' digits precision
	static std::string d2s(double x, int n) {
		// Will not use C++ version ( https://stackoverflow.com/a/29200671 )

		char format[9] = "%.3f";
		char buf[50];
		format[2] = n + '0';
		sprintf(buf, format, (float)x);
		return std::string(buf);
	}

public:
	void insert(double val) {
		if (m_count == 0) {
			m_min = m_max = m_sum = val;
			m_squareSum = (val*val);
			m_count = 1;
		}
		else {
			if (val < m_min) {
				m_min = val;
			}
			else if (val > m_max) {
				m_max = val;
			}
			m_sum += val;
			m_squareSum += (val*val);
			m_count++;
		}
	}

	int getCount() const { return m_count; }
	inline double getMin() const { return m_min; }
	inline double getMax() const { return m_max; }

	// Average: E(X)
	inline double getAvg() const { return m_sum / (double)m_count; }

	// Variance: E((X-E(X))^2) = E(X^2 - 2E(X)X + E(X)^2) = E(X^2) - E(X)^2
	double getVariance() const { return getAvgOfSqr() - sqr(getAvg()); }

	inline double getStdDev() const { return sqrt(getVariance()); }

	// String description, values are printed as integers
	std::string descriptionI() const {
		using namespace std;
		return "(min=" + to_string(lround(getMin())) + ", avg=" + to_string(lround(getAvg())) + ", max=" + to_string(lround(getMax()))
			+ ", stddev=" + to_string(lround(getStdDev())) + ")";
	}

	// String description, values are printed with precision (must be between 1 and 9)
	std::string descriptionP(int precision) const {
		using namespace std;
		assert(precision >= 1);
		assert(precision <= 9);
		return "(min=" + d2s(getMin(), precision) + ", avg=" + d2s(getAvg(), precision) + ", max=" + d2s(getMax(), precision)
			+ ", stddev=" + d2s(getStdDev(), precision) + ")";
	}
};


class MyProgram
{
public:
	int run(const vector<string>& commandLineArguments);

private:
	W4Strategy_p parsePlayerSpec(const std::string& spec);
	int runSingleGame();
	void saveStatistic(int numberOfAllocations, int numberOfBoards, int searchDepth, double duration);

	W4Strategy_p blackPlayer;
	W4Strategy_p redPlayer;

	bool testFlagSet = false;

	// Wie oft hat schwarz gewonnen?
	int blackWins = 0;
	// Wie oft hat rot gewonnen?
	int redWins = 0;
	// Wie oft war unentschieden?
	int ties = 0;

	bool alphaBetaPruningEnabled = true;
	uint64_t randomSeed = 0;
	std::shared_ptr<W4Random> prng;

	bool m_verboseStats = true;
	StatVar m_stat_allocations;
	StatVar m_stat_memoryConsumption;
	StatVar m_stat_duration;
	// in millisekunden
	StatVar m_stat_durationPerAllocation;
};

W4Strategy_p MyProgram::parsePlayerSpec(const std::string& spec) {
	if (spec == "human") {
		W4HumanPlayerStrategy *hp = new W4HumanPlayerStrategy();
		return W4Strategy_p(hp);
	}
	if (strStartsWith(spec, "human:")) {
		W4HumanPlayerStrategy *hp = new W4HumanPlayerStrategy();
		hp->setName(spec.substr(6));
		return W4Strategy_p(hp);
	}

	int kiType = 0;
	if (strStartsWith(spec, "ai-")) {
		kiType = 1;
	}
	else if (strStartsWith(spec, "ci-")) {
		kiType = 2;
	}
	else if (strStartsWith(spec, "pi-")) {
		kiType = 3;
	}

	if (kiType > 0) {
		string depthStr = spec.substr(3);
		int depth = -1;
		try {
			depth = stoi(depthStr);
		}
		catch (const std::exception& ex) {
			MDebug("Caught exception while trying to parse (AI) player description '%s': %s", spec.c_str(), ex.what());
		}
		if (depth < 1 || depth > 99) {
			return nullptr;
		}

		W4Andromeda* kiGegner = new W4Andromeda();

		kiGegner->m_optionDepth = depth;
		kiGegner->m_optionRandomness = 2;
		kiGegner->m_optionDilutionPercent = 2;
		kiGegner->m_optionAttenuationPercent = 10;
		kiGegner->m_optionEnableAlphaBetaPruning = alphaBetaPruningEnabled;

		const char* heuristicName = "";

		if (kiType == 1) {
			kiGegner->m_heuristic = std::shared_ptr<W4Heuristic>(new W4SmartHeuristic());
			heuristicName = "smart";
		}
		else if (kiType == 2) {
			kiGegner->m_heuristic = std::shared_ptr<W4Heuristic>(new W4HeuristicCenter());
			heuristicName = "center";
		}
		else if (kiType == 3) {
			kiGegner->m_heuristic = std::shared_ptr<W4Heuristic>(new W4SmartHeuristic());
			heuristicName = "pi";

			kiGegner->m_optionDilutionPercent = 0;
			kiGegner->m_optionAttenuationPercent = 0;
			kiGegner->m_optionRandomness = 0;
		}

		kiGegner->m_rng = prng;

		MDebug("Created AI with depth=%d (heuristic=%s)", depth, heuristicName);

		return W4Strategy_p(kiGegner);
	}

	return nullptr;
}

int MyProgram::run(const vector<string>& commandLineArguments)
{
	int idx = 0;
	int N = (int)commandLineArguments.size();

	// Parse options
	while (idx < N) {
		const string& opt = commandLineArguments[idx];
		if (!strStartsWith(opt, "/")) {
			break;
		}
		if (opt == "/verbose") {
			_MDebug_Enabled = true;
		}
		else if (opt == "/no-ab") {
			alphaBetaPruningEnabled = false;
		}
		else if (opt == "/help" || opt == "/?" || opt == "--help") {
			usage();
			return 1;
		}
		else if (strStartsWith(opt, "/seed=")) {
			string seedStr = opt.substr(6);
			try {
				randomSeed = stoll(seedStr);
			}
			catch (const std::exception& ex) {
				printf("Error: Cannot parse seed parameter: %s\n", ex.what());
				return 1;
			}
		}
		else if (opt == "/test") {
			testFlagSet = true;
		}
		else {
			printf("Error: Unknown command line option '%s'", opt.c_str());
			return 1;
		}
		idx++;
	}
	if (testFlagSet) {
		m_verboseStats = false;
	}
	if (randomSeed == 0) {
		randomSeed = std::time(0) * 468798123456981327L;
	}
	prng = std::shared_ptr<W4Random>(new W4RandomXorshift128(randomSeed));

	// Parse player specs
	if (idx + 2 > N) {
		printf("Error: too few arguments.\n");
		usage();
		return 1;
	}
	const std::string& spec1 = commandLineArguments[idx];
	const std::string& spec2 = commandLineArguments[idx + 1];
	blackPlayer = parsePlayerSpec(spec1);
	if (blackPlayer == nullptr) {
		printf("Error: Cannot parse first player spec '%s'\n", spec1.c_str());
	}
	redPlayer = parsePlayerSpec(spec2);
	if (redPlayer == nullptr) {
		printf("Error: Cannot parse second player spec '%s'\n", spec2.c_str());
	}
	if (blackPlayer == nullptr || redPlayer == nullptr) {
		return 1;
	}
	
	int returnValue;
	if (testFlagSet) {
		for (int i = 0; i < 100; i++) {
			runSingleGame();
		}
		printf("Statistics: schwarz: %3d Mal gewonnen, rot: %3d Mal gewonnen. Unentschieden: %3d Mal\n", blackWins, redWins, ties);
		returnValue = 0;
	}
	else {
		returnValue = runSingleGame();
	}

	if (m_stat_allocations.getCount() > 0) {
		printf("Statistics: allocations pro Zug: %s\n", m_stat_allocations.descriptionI().c_str());
		printf("Statistics: Memory consumption pro Zug in Bytes: %s\n", m_stat_memoryConsumption.descriptionI().c_str());
		printf("Statistics: Dauer pro Zug in Sekunden: %s\n", m_stat_duration.descriptionP(4).c_str());
		printf("Statistics: Dauer pro Zug durch Anzahl Allokationen, in ms: %s\n", m_stat_durationPerAllocation.descriptionP(5).c_str());
	}
}

int MyProgram::runSingleGame()
{
	using namespace std::chrono;

	int moveCounter = 1;
	W4Board_p board = W4Board::emptyBoard();

	if (!testFlagSet) {
		cout << "4 Gewinnt\n";
		cout << "(Code von Michael Peternell)\n";
		cout << "(Random Seed: " << randomSeed << ")\n";
		cout << "\n";
		cout << "Spielfeld:\n";
		board->printBoard();
		cout << endl;
	}
	
	for (int moveCounter = 1; moveCounter <= (NUM_ROWS*NUM_COLUMNS); moveCounter++) {
		W4Color werIstDran = board->getColor();
		W4Strategy_p player;
		string playerColorDescription;
		if (werIstDran == COLOR_BLACK) {
			player = blackPlayer;
			playerColorDescription = "Schwarz ('o')";
		}
		else if (werIstDran == COLOR_RED) {
			player = redPlayer;
			playerColorDescription = "Rot ('X')";
		}

		if (player->getPlayerType() == "human") {
			string name = player->getName();
			if (name == "") {
				cout << "In welche Spalte wollen Sie setzen, Spieler " << playerColorDescription << " ? ";
			}
			else {
				cout << "In welche Spalte wollen Sie setzen, " << name << " [" << playerColorDescription << "] ? ";
			}
		}

		W4Board::resetLiveMaxInstanceCounter();
		uint64_t mem_allocCountBefore = W4Board::instanceAllocationCounter();
		high_resolution_clock::time_point t_beforeMove = high_resolution_clock::now();

		int column = player->nextMove(board);

		high_resolution_clock::time_point t_afterMove = high_resolution_clock::now();
		double t_duration = (duration_cast<duration<double>>(t_afterMove - t_beforeMove)).count();
		uint64_t mem_allocCountAfter = W4Board::instanceAllocationCounter();
		int mem_numberOfAllocations = int(mem_allocCountAfter - mem_allocCountBefore);
		int mem_maxAllocations = W4Board::liveMaxInstanceCounter();

		if (player->getPlayerType() != "human") {
			W4Andromeda *ki = dynamic_cast<W4Andromeda *>(player.get());
			int depth = ki->m_optionDepth;

			saveStatistic(mem_numberOfAllocations, mem_maxAllocations, depth, t_duration);
		}
		
		if (column < 0) {
			cout << "Interner Fehler!\n";
			return 2;
		}
		if (player->getPlayerType() != "human" && !testFlagSet) {
			cout << "Spieler " << playerColorDescription << " (eine KI) setzt in Spalte # " << (column + 1) << "\n";
		}
		board = board->next(column);
		if (board == nullptr) {
			cout << "Oh no, ein interner Fehler ist aufgetreten. Das neue Board konnte nicht berechnet werden!\n";
			return 1;
		}
		if (!testFlagSet) {
			board->printBoard();
		}
		
		if (board->hasWon()) {
			if (werIstDran == COLOR_BLACK) {
				blackWins++;
			}
			else if (werIstDran == COLOR_RED) {
				redWins++;
			}
			if (player->getPlayerType() == "human") {
				string name = player->getName();
				if (name == "") {
					cout << "Gratuliere Mensch! Spieler " << playerColorDescription << " hat gewonnen!\n";
				}
				else {
					cout << "Gratuliere " << name << ": Sie haben gewonnen!\n";
				}
			}
			else {
				cout << "Die KI (" << playerColorDescription << ") hat gewonnen!\n";
			}
			return 0;
		}
		if (!testFlagSet) {
			cout << endl;
		}
	}

	cout << "*** Unentschieden! ***\n";
	ties++;

	return 0;
}

void MyProgram::saveStatistic(int numberOfAllocations, int numberOfBoards, int searchDepth, double duration)
{
	// Assume 64 bytes per stack frame for W4Andromeda::evalBoard() method.
	int stackSpace = searchDepth * 64;

	// Assume 16 bytes overhead per W4Board object.
	int isize_board = sizeof(W4Board) + 16;

	int heapSpace = numberOfBoards * isize_board;

	int usedMemory = stackSpace + heapSpace;

	if (m_verboseStats) {
		printf("Memory: allocations=%d, #boards=%d, memory=%d bytes, duration=%.4fs\n", numberOfAllocations, numberOfBoards, usedMemory, (float)duration);
	}

	m_stat_allocations.insert(numberOfAllocations);
	m_stat_memoryConsumption.insert(usedMemory);
	m_stat_duration.insert(duration);
	if (numberOfAllocations > 0) {
		m_stat_durationPerAllocation.insert(duration / (double)numberOfAllocations * 1000.0);
	}
}

int main(int argc, char **argv)
{
	if (argc < 3) {
		usage();
		return 1;
	}

	vector<string> commandLineArguments;
	for (int i = 1; i < argc; i++) {
		commandLineArguments.push_back(string(argv[i]));
	}

	MyProgram p;

	int returnValue = p.run(commandLineArguments);
	// Put breakpoint here for debugging.
	return returnValue;
}


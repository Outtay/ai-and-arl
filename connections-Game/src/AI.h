#ifndef AI_H
#define AI_H


#include <vector>
#include <iostream>


#include "Board.h"


class AI {
    
    public:
        AI(Board &currentBoard);
        void updateTmpBoard();

        int Heuristic(Board &board, int alpha, int beta, int maxDepth);

    //private: 
    public:
        Board *m_currentBoardPtr;
        Board m_tmpBoard;


        //returns a value based on the number and "quality" of paths that are present on the board
        //IMPORTANT: when a new move is made, only call this if the winning condition has been checked
        int pathHeuristic(Board &tmpBoard, std::vector<int> &visited);

        //returns a value based on the proximity to the middle
        int centerHeuristic(Board &tmpBoard);


        //Evaluates the Board for the current player
        int BoardEvaluation(Board &tmpBoard);
        Board::Position BestMove(Board &tmpBoard);

        Board::Position dummyAIcall();

        Board copyBoardFromCurrent();



};



#endif

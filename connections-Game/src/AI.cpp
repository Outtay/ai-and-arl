#include "AI.h"


AI::AI(Board &currentBoard)
    //:m_currentBoard(currentBoard)
{ 
    //m_tmpBoard(currentBoard);
    m_currentBoardPtr = &currentBoard;
}

Board AI::copyBoardFromCurrent(){
    Board tmp = *m_currentBoardPtr;
    //setState changes the pointers from the copied object
    tmp.setState(tmp.m_currentPlayer);
    return tmp;
}

void AI::updateTmpBoard(){
    m_tmpBoard = *m_currentBoardPtr;
    m_tmpBoard.setState(m_tmpBoard.m_currentPlayer);

}

std::vector<Board::Position> possibleMoves(Board &tmpBoard){

    std::vector<Board::Position> result;
    //copied from firstLegalMove
    
    //This one is for horizontal Mode
    //because of the tmpId2 = i+1 end for loop sooner
    for (size_t i = 0; i < 29; i++){
        int tmpId1 = i;
        int tmpId2 = i+1;
        if(tmpBoard.MovePossible(tmpId1, tmpId2)){
            result.push_back({tmpId1, tmpId2});
        }
    }

    //This one is for vertical Mode
    for (size_t i = 0; i < 30; i++){
        int tmpId1 = i;
        int tmpId2 = i + tmpBoard.m_playerBoardWidth;
        if( (tmpId1 < (tmpBoard.m_playerBoardWidth * tmpBoard.m_playerBoardHeight - 1 ) 
                    && tmpBoard.MovePossible(tmpId1, tmpId2))){
            result.push_back({tmpId1, tmpId2});
        }
    }

    return result;

}


//TODO simple optimization: use some sort of look-up table to only look at
//moves that are valid moves
//TODO other optimization: don't create a whole new real board for every iteration
Board::Position AI::BestMove(Board &tmpBoard){

    Board::Position bestMove;
    int bestValue = 9000;
    int alpha = -9000;
    int beta = 9000;
    int depth = 2;

    
    //std::cout << "Possible Moves:" << std::endl;

    std::vector<Board::Position> possibleMoveVector = possibleMoves(tmpBoard);

 
    for (size_t i = 0; i < possibleMoveVector.size(); i++){
        //std::cout << possibleMoveVector[i].id1 << "," << possibleMoveVector[i].id2 << std::endl;
        Board newBoard = tmpBoard;
        newBoard.setState(newBoard.m_currentPlayer);
        newBoard.addConnection(possibleMoveVector[i].id1, possibleMoveVector[i].id2, Board::PIECES::REAL_PIECE);

        int value = Heuristic(newBoard, alpha, beta, depth - 1);

        //std::cout << "Value: " << value << std::endl;
        //std::cout << "Move: " << possibleMoveVector[i].id1 << "," << possibleMoveVector[i].id2 << std::endl;
        if (value < bestValue){
            bestValue = value;
            bestMove = {possibleMoveVector[i].id1, possibleMoveVector[i].id2};
        }
            
    }

    //int id1 = 0, id2 = 1;
    //tmpBoard.FirstLegalMove(id1, id2);
    return bestMove;
}

int AI::Heuristic(Board &board, int alpha, int beta, int maxDepth){

    bool alphaBetaPruningEnabled = false;
    
    if (board.checkWinningCondition()){
        return -5000;
    }
    if (maxDepth <= 0){

        int result = 0;
        result += centerHeuristic(board);
        return result * board.getCurrentPlayer();
        
    }

    int maxValue = -5000;


    std::vector<Board::Position> possibleMoveVector = possibleMoves(board);
 
    for (size_t i = 0; i < possibleMoveVector.size(); i++){
        Board newBoard = board;
        newBoard.setState(newBoard.m_currentPlayer);
        newBoard.addConnection(possibleMoveVector[i].id1, possibleMoveVector[i].id2, Board::PIECES::REAL_PIECE);
        newBoard.setState(newBoard.m_otherPlayer);

        int nextValue = -Heuristic(newBoard, -beta, -alpha, maxDepth - 1);

        if (nextValue > maxValue){
            maxValue = nextValue;
        }

        if (alphaBetaPruningEnabled){
            if (nextValue >= beta){
                break;
            }
            if (nextValue > alpha){
                alpha = nextValue;
            }
        }

    }  

    return maxValue;

}


Board::Position AI::dummyAIcall(){
    updateTmpBoard();
    return BestMove(m_tmpBoard);
}


int AI::pathHeuristic(Board &tmpBoard, std::vector<int> &visited){
    int result = 0;
    
    //visited.push_back(currentNode);
    for(size_t i = 0; i < (tmpBoard.m_currentGraphPtr)->size(); i++){

        for(size_t j = 0; j < (tmpBoard.m_currentGraphPtr)[i].size(); j++){

        }
        
        /*if(vectorHelper_Exists( (*m_currentGraphPtr)[currentNode][i], visited))
            continue;

        if(DFSPathExists( (*m_currentGraphPtr)[currentNode][i], goalNode, visited))
            return true;*/

    }
    visited.pop_back();



    return result;
}

//Heuristic based mostly on the proximity to the middle:
//  if an edge contains an index mod width or at the other border => smallest modifier
//  if an edge does not contain that => bigger modifier
//  return the indices times the modifier, of course this means that positions in the lower right
//  are always preferred 
//  TODO see if that is really that bad

int AI::centerHeuristic(Board &tmpBoard){
    int result = 0;
    int outerModifier = 1;
    int innerModifier = 2;
    int width = tmpBoard.m_playerBoardWidth;
    //int height = tmpBoard.m_playerBoardHeight;
    std::vector<std::vector <int> > *graphPtr = tmpBoard.m_currentGraphPtr;
    
    for(size_t i = 0; i < graphPtr->size(); i++){

        for(size_t j = 0; j < (*graphPtr)[i].size(); j++){
            int tmp1 = i;
            int tmp2 = (*graphPtr)[i][j];
            if (tmp1 % width == 0 || tmp1 < width || tmp1 > 30 - width || (tmp1+1) % width == 0 ||
                tmp2 % width == 0 || tmp2 < width || tmp2 > 30 - width || (tmp2+1) % width == 0){
                result += (tmp1 + tmp2) * outerModifier;
            }
            else {
                result += (tmp1 + tmp2) * innerModifier;
            }
            
        }
        
        /*if(vectorHelper_Exists( (*m_currentGraphPtr)[currentNode][i], visited))
            continue;

        if(DFSPathExists( (*m_currentGraphPtr)[currentNode][i], goalNode, visited))
            return true;*/

    }

    //std::cout << "Value from CenterHeuristic: " << result << std::endl; 

    return result;
    
}

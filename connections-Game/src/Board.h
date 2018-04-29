#ifndef BOARD_H
#define BOARD_H


#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <iterator>


extern bool g_GAME_IS_WON;


class Board{
    
    static const size_t WINDOW_PADDING = 50;

    static const int MAX_VALUE = 900000;
    
    public:
        struct Movement{
            enum Enum{
                RIGHT,
                LEFT,
                UP,
                DOWN
            };
        };
        
        enum Player{
            RED = -1,
            WHITE = 1
        };
        
        enum PIECES {
            VIRTUAL_PIECE,
            REAL_PIECE
        };
        
        struct Position{
            int id1;
            int id2;
        };

        Board();
        explicit Board(int xStart, int yStart, int size);
        ~Board();

        void RenderBoard(sf::RenderWindow &window);        
        void CommitMove();
        void CommitMoveAI(Board::Position pos);
        void ShowMove(Movement::Enum e);

        //change from horizontal to vertical and vice-versa
        void toggleMode();

        bool checkWinningCondition();

        void PerformVictory();

        Player getCurrentPlayer(){
            return m_currentPlayer;
        }

        //add a connection between two nodes and depending on whether it's real or not
        //add it to the graph
        void addConnection(int id1, int id2, PIECES piece);
        
        void setState(Player player);


    //private:
    public:

        //-----------Member Variables------------
        
        int m_xStart;
        int m_yStart;
        int m_boardSize;
    
        float m_SquareSize = 10.0f;
        bool m_horizontalMode = true;

        //Everything's modelled like graph, so I do not place tiles, but rather
        //edges with a beginning and an end
        int m_currentMoveBegin = 0;
        int m_currentMoveEnd = 1;

        std::vector<sf::CircleShape> m_debugCircles;
        std::vector<sf::Text> m_text;

        sf::Font m_font;

        //The state is stored in variables and upon switching the player 
        //pointers and variables are switched for the other player
        std::vector< std::vector <int> > * m_currentGraphPtr;
        std::vector< std::vector <int> > * m_oppositeGraphPtr;
        std::vector< std::vector <int> > m_whiteSquare_Graph;
        std::vector< std::vector <int> > m_redSquare_Graph;
        
        Player m_currentPlayer;
        Player m_otherPlayer;

        int m_playerBoardWidth;
        int m_playerBoardHeight;

        sf::CircleShape * m_currentSquarePtr;
        sf::CircleShape m_whiteSquares[30];
        sf::CircleShape m_redSquares[30];

        sf::RectangleShape * m_currentEdgePtr;
        size_t * m_currentEdgeSizePtr;
        size_t m_whiteEdgesSize = 0;
        sf::RectangleShape m_whiteEdges[20];
        sf::RectangleShape * m_opposingEdgePtr;
        size_t * m_opposingEdgeSizePtr;
        size_t m_redEdgesSize = 0;
        sf::RectangleShape m_redEdges[20];

        sf::Color m_currentColor;

        
        //---------Methods------------
        
        
        void removeLastVirtualConnection();

        bool ConnectionExists(int id1, int id2);

        //Helper Function for MovePossible
        bool CrossingOpponent(int id1, int id2);

        //Finding the first legal move on the board and writing it into the id
        void FirstLegalMove(int &id1, int &id2);
        
        //Try the desired move as a modification of the current position and show it
        void PerformMove(int index1Modifier, int index2Modifier);

        bool MovePossible(int id1, int id2);

        void SwitchPlayers(Player player);


        bool DFSCycles(int start, int node, std::vector<int> &visited);
        bool DFSPathExists(int currentNode, int goalNode, std::vector<int> &visited);


        //Strictly AI - Stuff: waiting for refactor
        
        void createTmpFromCurrent(Board &current);

        //returns the length of the path formed by the last piece
        //IMPORTANT: if winning condition is not checked beforehand, this might result in an
        //infinite loop due to cycles
        int pathLengthLastPosition(Position lastPos, Board &tmpBoard);

        //returns a value based on the number and "quality" of paths that are present on the board
        //IMPORTANT: when a new move is made, only call this if the winning condition has been checked
        int pathHeuristic(Board &tmpBoard, std::vector<int> &visited);

        //returns a value based on the proximity to the middle
        int centerHeuristic(Board &tmpBoard);


        //Evaluates the Board for the current player
        int BoardEvaluation(Board &tmpBoard);
        Position BestMove(Board &tmpBoard);

        //END OF AI STUFF


};

#endif

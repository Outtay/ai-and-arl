#ifndef BOARD_H
#define BOARD_H


#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>



class Board{
    
    static const size_t WINDOW_PADDING = 50;
    
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
            RED = 0,
            WHITE = 1
        };
        
        enum Virtual {
            VIRTUAL_PIECE,
            REAL_PIECE
        };


        explicit Board(int xStart, int yStart, int size);
        ~Board();

        void RenderBoard(sf::RenderWindow &window);        
        void CommitMove();
        void ShowMove(Movement::Enum e);

        //change from horizontal to vertical and vice-versa
        void toggleMode();

        Player getCurrentPlayer(){
            return m_currentPlayer;
        }

        

    private:

        //-----------Member Variables------------
    
        float m_SquareSize = 10.0f;
        bool m_horizontalMode = true;

        //Everything's modelled like graph, so I do not place tiles, but rather
        //edges with a beginning and an end
        int m_currentMoveBegin = 0;
        int m_currentMoveEnd = 1;

        std::vector<sf::CircleShape> debugCircles;

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
        
        //add a connection between to nodes and depending on whether it's real or not
        //add it to the graph
        void addConnection(int id1, int id2, Virtual piece);

        void removeLastVirtualConnection();

        bool ConnectionExists(int id1, int id2);

        bool CrossingOpponent(int id1, int id2);

        //Finding the first legal move on the board and writing it into the id
        void FirstLegalMove(int &id1, int &id2);
        
        //Try the desired move and see if it works
        void PerformMove(int index1Modifier, int index2Modifier);

        bool MovePossible(int id1, int id2);

        void SwitchPlayers(Player player);

        void setState(Player player);


};

#endif

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



        explicit Board(int xStart, int yStart, int size){
            
            /*int viewSquare = std::min(windowHeight - WINDOW_PADDING, windowWidth- WINDOW_PADDING);
            int verticalOffset = windowHeight - viewSquare;
            int horizontalOffset = windowWidth - viewSquare;


            int hor_stepsize = m_stepModifier * (viewSquare - 30 * m_SquareSize)/6;
            int ver_stepsize = m_stepModifier * (viewSquare - 30 * m_SquareSize)/5;

            int totalsize = ver_stepsize * 5;
            int xStart = (windowWidth-totalsize)/2;
            int yStart = (windowHeight-totalsize)/2;*/
            
            int hor_stepsize = (size - 5 * m_SquareSize )/5;
            int ver_stepsize = (size - 6 * m_SquareSize )/6;

            for(int i = 0; i < 6; i++){
                for (int j = 0; j < 5; j++){
                    sf::CircleShape tmp(m_SquareSize, 4);
                    tmp.setPosition( xStart + j * hor_stepsize, yStart + i * ver_stepsize);
                    tmp.rotate(45.0f);
                    //5 is the width of the board
                    m_whiteSquares[i*5+j] = tmp;
                }

            }

            int redOffsetX = -hor_stepsize/2 + 2;
            int redOffsetY = ver_stepsize/2 + 2;
            
            for(int i = 0; i < 5; i++){
                for (int j = 0; j < 6; j++){
                    sf::CircleShape tmp(m_SquareSize, 4);
                    tmp.setPosition(xStart + j * hor_stepsize + redOffsetX, yStart+ i * ver_stepsize + redOffsetY);
                    tmp.rotate(45.0f);
                    tmp.setFillColor(sf::Color(255,0,0));
                    //5 is the width of the board
                    m_redSquares[i*6+j] = tmp;
                }

            }

            m_whiteSquare_Graph = std::vector< std::vector<int> >(n);
            m_redSquare_Graph = std::vector< std::vector<int> > (n);


            /*addConnection(0,1, true, false);
            addConnection(1,2, true, false);
            addConnection(0,5, true, false); 
            addConnection(2,7, true, false);
            addConnection(0,1, false, false);
            addConnection(23,29, false, false);
            addConnection(22,23, false, false);
            */
        }

        ~Board(){
        }

        void RenderBoard(sf::RenderWindow &window){
            for(int i = 0; i < 30; i++){
                window.draw(m_whiteSquares[i]);
                window.draw(m_redSquares[i]);
            } 
            for(size_t i = 0; i < m_whiteEdgesSize; i++){
                window.draw(m_whiteEdges[i]);
                sf::CircleShape tmp (5, 6);
                tmp.setPosition(m_whiteEdges[i].getPosition());
                tmp.setFillColor(sf::Color(255,0,0));
                window.draw(tmp);
            }
            for(size_t i = 0; i < m_redEdgesSize; i++){
                window.draw(m_redEdges[i]);
                sf::CircleShape tmp(5,6);
                tmp.setPosition(m_redEdges[i].getPosition());
                window.draw(tmp);
            }

            for(size_t i = 0; i < debugCircles.size(); i++){
                window.draw(debugCircles[i]);
            }
        }


        void addConnection(int id1, int id2, bool white, bool hologram){
            
            setPointers(white);
            /*if(white)
                std::cout << "white";
            else 
                std::cout << "red";*/

            if(!hologram){
                (*m_currentGraphPtr)[id1].push_back(id2);
                (*m_currentGraphPtr)[id2].push_back(id1);
            }
            
            sf::Vector2f pos1 = m_currentSquarePtr[id1].getPosition();
            sf::Vector2f pos2 = m_currentSquarePtr[id2].getPosition();
            float distance;
            //using the rectangleshape for connections
            sf::RectangleShape rect;
            //There are only two cases for connections:
            //horizontal or vertical
            if (id1 % 5 == id2 % 5){
                distance = std::abs(pos1.y - pos2.y);
                //using a magic number for fitting offsets. Haven't figured out the logic yet
                distance -= m_SquareSize + 20.5f;
                rect.setSize(sf::Vector2f(9.0f, distance));
                //using magic numbers again, since couldn't see the logic
                rect.setPosition(pos1.x - 5.0f, pos1.y + 30.0f);
            }
            else {
                distance = std::abs(pos1.x - pos2.x);
                distance -= m_SquareSize + 20.5f;
                rect.setSize(sf::Vector2f(distance, 9.0f));
                rect.setPosition(pos1.x + 15.0f, pos1.y + 10.0f);
            }
            rect.setFillColor(m_currentColor);
            m_currentEdgePtr[(*m_currentEdgeSizePtr)] = rect;
            (*m_currentEdgeSizePtr)++;
            
        }

        void removeLastVirtualConnection(bool white){
            setPointers(white);
            std::cout << (*m_currentEdgeSizePtr) << std::endl;
            (*m_currentEdgeSizePtr)--;
            std::cout << (*m_currentEdgeSizePtr) << std::endl;
        }

        bool ConnectionExists(int id1, int id2, bool white){
            setPointers(white);
            if (white){
                for (size_t i = 0; i < (*m_currentGraphPtr)[id1].size(); i++){
                   if ((*m_currentGraphPtr)[id1][i] == id2){
                       return true;
                   }
                }
            }
            return false;
        }
        //Not the most elegant check to see if there's an illegal intersection
        //simply check their bounding boxes. Would've loved something more precise
        //with graphs, but that would've probably required a look-up table of some sort
        bool CrossingOpponent(int id1, int id2, bool white){
            setPointers(white);
            addConnection(id1, id2, white, true);

            //sf::Vector2f lastConnectionPos = m_currentEdgePtr[(*m_currentEdgeSizePtr)-1].getPosition();
            sf::FloatRect lastConnectionBounds = m_currentEdgePtr[(*m_currentEdgeSizePtr)-1].getGlobalBounds();
            
            for (size_t i = 0; i < *m_opposingEdgeSizePtr; i++){
                /*std::cout << lastConnectionPos.x << ", " << lastConnectionPos.y << std::endl;
                sf::Vector2f tempPos = m_opposingEdgePtr[i].getPosition();
                std::cout << tempPos.x << ", " << tempPos.y << std::endl;
                */
                if(lastConnectionBounds.intersects(m_opposingEdgePtr[i].getGlobalBounds())){
                    std::cout << "intersecting";
                    return true;
                }

            }

            return false;
        }

        //using the ids as return values
        void FirstLegalMove(int &id1, int &id2, bool white){
            setPointers(white);



            
        }


        void ShowMove(Movement::Enum e, bool white){
            //setPointers(white);
            switch(e){
                case Movement::RIGHT:
                    if(m_horizontalMode){
                        if(!ConnectionExists(0,5,white)){
                            addConnection(0,5, white,true);
                            //addConnection(0,1, !white, true);
                            if(CrossingOpponent(0,1, !white))
                                removeLastVirtualConnection(!white);
                        }
                    }
                    
                    break;
                case Movement::LEFT:
                    break;
                default:
                    break;
            }
            


        }

        void toogleMode(){
            m_horizontalMode = ~m_horizontalMode;
        }

        

    private:
        int n = 30;
        int m_BoardSize;
        int m_BoardOrigin;
        int m_stepModifier = 1;
        float m_SquareSize = 10.0f;
        bool m_horizontalMode = true;
        struct Square{
            int posX;
            int posY;
            sf::Color col;
        };

        std::vector<sf::CircleShape> debugCircles;

        std::vector< std::vector <int> > * m_currentGraphPtr;
        std::vector< std::vector <int> > * m_oppositeGraphPtr;
        std::vector< std::vector <int> > m_whiteSquare_Graph;
        std::vector< std::vector <int> > m_redSquare_Graph;

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

        void setPointers(bool white){
            if (white){
                m_currentGraphPtr = &m_whiteSquare_Graph;
                m_oppositeGraphPtr = &m_redSquare_Graph;
                m_currentSquarePtr = &m_whiteSquares[0];
                m_currentEdgePtr = &m_whiteEdges[0];
                m_currentEdgeSizePtr = &m_whiteEdgesSize;
                m_opposingEdgePtr = &m_redEdges[0];
                m_opposingEdgeSizePtr = &m_redEdgesSize;
                m_currentColor = sf::Color(255,255,255);
            } else {
                m_currentGraphPtr = &m_redSquare_Graph;
                m_oppositeGraphPtr = &m_whiteSquare_Graph;
                m_currentSquarePtr = &m_redSquares[0];
                m_currentEdgePtr = &m_redEdges[0]; 
                m_currentEdgeSizePtr = &m_redEdgesSize;
                m_opposingEdgePtr = &m_whiteEdges[0];
                m_opposingEdgeSizePtr = &m_whiteEdgesSize;
                m_currentColor = sf::Color(255,0,0);
            }
        }



};

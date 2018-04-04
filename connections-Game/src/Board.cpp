#include "Board.h"


Board::Board(int xStart, int yStart, int size){

            /*int viewSquare = std::min(windowHeight - WINDOW_PADDING, windowWidth- WINDOW_PADDING);
            int verticalOffset = windowHeight - viewSquare;
            int horizontalOffset = windowWidth - viewSquare;


            int hor_stepsize = m_stepModifier * (viewSquare - 30 * m_SquareSize)/6;
            int ver_stepsize = m_stepModifier * (viewSquare - 30 * m_SquareSize)/5;

            int totalsize = ver_stepsize * 5;
            int xStart = (windowWidth-totalsize)/2;
            int yStart = (windowHeight-totalsize)/2;*/
            
            // Draw the white squares first
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

            //Draw the red squares next
            int redOffsetX = -hor_stepsize/2 + 2;
            int redOffsetY = ver_stepsize/2 + 2;
            
            for(int i = 0; i < 5; i++){
                for (int j = 0; j < 6; j++){
                    sf::CircleShape tmp(m_SquareSize, 4);
                    tmp.setPosition(xStart + j * hor_stepsize + redOffsetX, yStart+ i * ver_stepsize + redOffsetY);
                    tmp.rotate(45.0f);
                    tmp.setFillColor(sf::Color(255,0,0));
                    //6 is the width of the board
                    m_redSquares[i*6+j] = tmp;
                }

            }

            m_whiteSquare_Graph = std::vector< std::vector<int> >(30);
            m_redSquare_Graph = std::vector< std::vector<int> > (30);

            setState(Player::WHITE);

            //This already starts the game and puts the first edge on the board
            FirstLegalMove(m_currentMoveBegin, m_currentMoveEnd);
            addConnection(m_currentMoveBegin, m_currentMoveEnd, Virtual::VIRTUAL_PIECE);
            
            /*setState(Player::RED);
            addConnection(0,6, Player::RED, true);
            setState(Player::WHITE);*/
}


Board::~Board(){
}


void Board::RenderBoard(sf::RenderWindow &window){
    for(int i = 0; i < 30; i++){
        window.draw(m_whiteSquares[i]);
        window.draw(m_redSquares[i]);
    } 
    for(size_t i = 0; i < m_whiteEdgesSize; i++){
        window.draw(m_whiteEdges[i]);
        /*sf::CircleShape tmp (5, 6);
        tmp.setPosition(m_whiteEdges[i].getPosition());
        tmp.setFillColor(sf::Color(255,0,0));
        window.draw(tmp);*/
    }
    for(size_t i = 0; i < m_redEdgesSize; i++){
        window.draw(m_redEdges[i]);
        /*sf::CircleShape tmp(5,6);
        tmp.setPosition(m_redEdges[i].getPosition());
        window.draw(tmp);*/
    }

    for(size_t i = 0; i < debugCircles.size(); i++){
        window.draw(debugCircles[i]);
    }
        
}



void Board::addConnection(int id1, int id2, Virtual piece){
    //Only write the connection into the graph if the piece is final        
    if(piece == Virtual::REAL_PIECE){
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
    if (id1 % m_playerBoardWidth == id2 % m_playerBoardWidth){
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



void Board::removeLastVirtualConnection(){
    (*m_currentEdgeSizePtr)--;
}


bool Board::ConnectionExists(int id1, int id2){
    for (size_t i = 0; i < (*m_currentGraphPtr)[id1].size(); i++){
       if ((*m_currentGraphPtr)[id1][i] == id2){
           return true;
       }
    }
    return false;
}


//Not the most elegant check to see if there's an illegal intersection:
//simply check their bounding boxes. Would've loved something more precise
//with graphs, but that would've probably required a look-up table of some sort
bool Board::CrossingOpponent(int id1, int id2){
    addConnection(id1, id2, Virtual::VIRTUAL_PIECE);

    sf::FloatRect lastConnectionBounds = m_currentEdgePtr[(*m_currentEdgeSizePtr)-1].getGlobalBounds();
    
    for (size_t i = 0; i < *m_opposingEdgeSizePtr; i++){
        if(lastConnectionBounds.intersects(m_opposingEdgePtr[i].getGlobalBounds())){
            removeLastVirtualConnection();
            return true;
        }

    }
    removeLastVirtualConnection();

    return false;
}


//using the ids as return values when searching for the first possible move on the board,
//giving preferential treatment to the previous location when dealing with 
void Board::FirstLegalMove(int &id1, int &id2){

    if (m_horizontalMode){
        //first find out if at the current Position there might
        //be a possible move 
        //This is useful for toggling between horizontal and vertical
        //and is quite a hack to prevent getting stuck somewhere
        //TODO: find out if that still might happen
        if ( MovePossible(id1, id1 + 1)){
            id2 = id1 + 1;
            return;
        }

        //because of the tmpId2 = i+1 end for loop sooner
        for (size_t i = 0; i < 29; i++){
            int tmpId1 = i;
            int tmpId2 = i+1;
            if(!ConnectionExists(tmpId1, tmpId2)){
                id1 = tmpId1;
                id2 = tmpId2;
                return;
            }
        }
    } else {
        //look at comment above
        if ( MovePossible(id1, id1 + m_playerBoardWidth)){
            id2 = id1 + m_playerBoardWidth;
            return;
        }

        for (size_t i = 0; i < 30; i++){
            int tmpId1 = i;
            int tmpId2 = i + m_playerBoardWidth;
            if( (tmpId1 < (m_playerBoardWidth * m_playerBoardHeight - 1 ) 
                        && !ConnectionExists(tmpId1, tmpId2))){
                id1 = tmpId1;
                id2 = tmpId2;
                return;
            }
        }
    }

}


void Board::SwitchPlayers(Player player){
    setState(player);
    m_horizontalMode = true;
    m_currentMoveBegin = 0;
    m_currentMoveEnd = 1;
    FirstLegalMove(m_currentMoveBegin, m_currentMoveEnd);
    addConnection(m_currentMoveBegin, m_currentMoveEnd, Virtual::VIRTUAL_PIECE);
}


//Finalize a move made by the user
void Board::CommitMove(){
    removeLastVirtualConnection();
    addConnection(m_currentMoveBegin, m_currentMoveEnd, Virtual::REAL_PIECE);
    SwitchPlayers(m_otherPlayer);
}



bool Board::MovePossible(int id1, int id2){
    if ( id1 < 0 || id2 < 0 || id1 >= 30 || id2 >= 30 ||
            (id1 % m_playerBoardWidth == m_playerBoardWidth - 1 && 
             id2 % m_playerBoardWidth == 0))
    {
        return false;
    }
    if(!ConnectionExists(id1, id2)){
        if(!CrossingOpponent(id1, id2)){
            return true;
        }
    }
    
    return false;
}


//Try to perform a move that is desired by the user
void Board::PerformMove(int index1Modifier, int index2Modifier){
    int testedMoveBegin = m_currentMoveBegin + index1Modifier; 
    int testedMoveEnd = m_currentMoveEnd +  index2Modifier;
    
    //There is a difference between an impossible move that's not possible due to
    while (testedMoveEnd < 30 && testedMoveBegin >= 0){
        if(MovePossible(testedMoveBegin, testedMoveEnd)){
            removeLastVirtualConnection();
            m_currentMoveBegin = testedMoveBegin;
            m_currentMoveEnd = testedMoveEnd;
            addConnection(m_currentMoveBegin, m_currentMoveEnd, Virtual::VIRTUAL_PIECE);
            break;
        } else {
            testedMoveBegin += index1Modifier;
            testedMoveEnd += index2Modifier;
        }
    }

}



void Board::ShowMove(Movement::Enum e){
    switch(e){
        case Movement::RIGHT:
            PerformMove( +1, +1);
            break;
        case Movement::LEFT:
            PerformMove( -1, -1);
            break;
        case Movement::UP:
            PerformMove( -m_playerBoardWidth, -m_playerBoardWidth);
            break;
        case Movement::DOWN:
            PerformMove( +m_playerBoardWidth, +m_playerBoardWidth);
            break;
        default:
            break;
    }

}



void Board::toggleMode(){
    m_horizontalMode = !m_horizontalMode;
    FirstLegalMove(m_currentMoveBegin, m_currentMoveEnd);
    //std::cout << m_currentMoveBegin << std::endl << m_currentMoveEnd;
    removeLastVirtualConnection();
    addConnection(m_currentMoveBegin, m_currentMoveEnd, Virtual::VIRTUAL_PIECE);
}



void Board::setState(Player player){
    if (player == Player::WHITE){
        m_currentGraphPtr = &m_whiteSquare_Graph;
        m_oppositeGraphPtr = &m_redSquare_Graph;
        m_currentSquarePtr = &m_whiteSquares[0];
        m_currentEdgePtr = &m_whiteEdges[0];
        m_currentEdgeSizePtr = &m_whiteEdgesSize;
        m_opposingEdgePtr = &m_redEdges[0];
        m_opposingEdgeSizePtr = &m_redEdgesSize;
        m_currentColor = sf::Color(255,255,255);
        m_otherPlayer = Player::RED;
        m_currentPlayer = Player::WHITE;
        m_playerBoardWidth = 5;
        m_playerBoardHeight = 6;
    } else {
        m_currentGraphPtr = &m_redSquare_Graph;
        m_oppositeGraphPtr = &m_whiteSquare_Graph;
        m_currentSquarePtr = &m_redSquares[0];
        m_currentEdgePtr = &m_redEdges[0]; 
        m_currentEdgeSizePtr = &m_redEdgesSize;
        m_opposingEdgePtr = &m_whiteEdges[0];
        m_opposingEdgeSizePtr = &m_whiteEdgesSize;
        m_currentColor = sf::Color(255,0,0);
        m_otherPlayer = Player::WHITE;
        m_currentPlayer = Player::RED;
        m_playerBoardWidth = 6;
        m_playerBoardHeight = 5;
    }
}

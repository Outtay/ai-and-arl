#include "Board.h"


Board::Board(int xStart, int yStart, int size)
    :m_xStart(xStart), m_yStart(yStart), m_boardSize(size)
{

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
            addConnection(m_currentMoveBegin, m_currentMoveEnd, PIECES::VIRTUAL_PIECE);

            m_font.loadFromFile("arial.ttf");
            
            /*setState(Player::RED);
            addConnection(0,6, Player::RED, true);
            setState(Player::WHITE);*/
}


Board::~Board(){
}

Board::Board(){

}


void Board::RenderBoard(sf::RenderWindow &window){
    for(int i = 0; i < 30; i++){
        window.draw(m_whiteSquares[i]);
        window.draw(m_redSquares[i]);
    } 
    for(size_t i = 0; i < m_whiteEdgesSize; i++){
        window.draw(m_whiteEdges[i]);
    }
    for(size_t i = 0; i < m_redEdgesSize; i++){
        window.draw(m_redEdges[i]);
    }

    for(size_t i = 0; i < m_debugCircles.size(); i++){
        window.draw(m_debugCircles[i]);
    }

    for(size_t i = 0; i < m_text.size(); i++){
        window.draw(m_text[i]);
    }

    /*// Declare and load a font
    sf::Font font;
    font.loadFromFile("arial.ttf");
    // Create a text
    sf::Text text("hello", font);
    text.setCharacterSize(30);
    text.setStyle(sf::Text::Bold);
    text.setFillColor(sf::Color::Red);

    window.draw(text);*/
        
}



void Board::addConnection(int id1, int id2, PIECES piece){
    //Only write the connection into the graph if the piece is final        
    if(piece == PIECES::REAL_PIECE){
        (*m_currentGraphPtr)[id1].push_back(id2);
        (*m_currentGraphPtr)[id2].push_back(id1);
        //Also add it to a general big graph
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
    if ((*m_currentEdgeSizePtr) > 0)
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
    addConnection(id1, id2, PIECES::VIRTUAL_PIECE);

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
//giving preferential treatment to the previous location
//id1 and id2 have to be at least initialized and should preferrably be valid
void Board::FirstLegalMove(int &id1, int &id2){

    if (m_horizontalMode){
        //first find out if at the current Position there might
        //be a possible move 
        //This is useful for toggling between horizontal and vertical
        if ( MovePossible(id1, id1 + 1)){
            id2 = id1 + 1;
            return;
        }

        //because of the tmpId2 = i+1 end for loop sooner
        for (size_t i = 0; i < 29; i++){
            int tmpId1 = i;
            int tmpId2 = i+1;
            if(MovePossible(tmpId1, tmpId2)){
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
                        && MovePossible(tmpId1, tmpId2))){
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
    addConnection(m_currentMoveBegin, m_currentMoveEnd, PIECES::VIRTUAL_PIECE);
}


//Finalize a move made by the user
void Board::CommitMove(){
    removeLastVirtualConnection();
    addConnection(m_currentMoveBegin, m_currentMoveEnd, PIECES::REAL_PIECE);
    if(checkWinningCondition())
        g_GAME_IS_WON = true;
    else
        SwitchPlayers(m_otherPlayer);
}

void Board::CommitMoveAI(Board::Position pos){
    removeLastVirtualConnection();
    addConnection(pos.id1, pos.id2, PIECES::REAL_PIECE);
    if(checkWinningCondition())
        g_GAME_IS_WON = true;
    else
        SwitchPlayers(m_otherPlayer);
}

void Board::CommitMoveAIToAI(Board::Position pos){
    addConnection(pos.id1, pos.id2, PIECES::REAL_PIECE);
    if(checkWinningCondition())
        g_GAME_IS_WON = true;
    else{
        setState(m_otherPlayer);
        m_horizontalMode = true;
        m_currentMoveBegin = 0;
        m_currentMoveEnd = 1;
        FirstLegalMove(m_currentMoveBegin, m_currentMoveEnd);
    }
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


//Try to perform a move that is desired by the user, but don't add it to the graph yet
void Board::PerformMove(int index1Modifier, int index2Modifier){
    int testedMoveBegin = m_currentMoveBegin + index1Modifier; 
    int testedMoveEnd = m_currentMoveEnd +  index2Modifier;
    
    //Packing that into a while provides us with the ability to wrap around and
    //jump over already existing tiles
    while (testedMoveEnd < 30 && testedMoveBegin >= 0){
        if(MovePossible(testedMoveBegin, testedMoveEnd)){
            removeLastVirtualConnection();
            m_currentMoveBegin = testedMoveBegin;
            m_currentMoveEnd = testedMoveEnd;
            addConnection(m_currentMoveBegin, m_currentMoveEnd, PIECES::VIRTUAL_PIECE);
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
    addConnection(m_currentMoveBegin, m_currentMoveEnd, PIECES::VIRTUAL_PIECE);
}


//Helper to find if a vector contains a certain element
bool vectorHelper_Exists(int el, std::vector<int> & vector){
    for(size_t i = 0; i < vector.size(); i++){
        if (el == vector[i]){
            return true;
        }
    }
    return false;
}


//Use Depth First Search in a very hacky way to find cycles
bool Board::DFSCycles(int start, int node, std::vector<int> &visited){

    if (visited.size() > 2 && node == start){
        return true;
    }

    if (node != start)
        visited.push_back(node);
    for( size_t i = 0; i < (*m_currentGraphPtr)[node].size(); i++){
        
        if ( ((*m_currentGraphPtr)[node][i] == start && visited.size() < 3) 
                || vectorHelper_Exists( (*m_currentGraphPtr)[node][i], visited))
            continue;

        if(DFSCycles(start, (*m_currentGraphPtr)[node][i], visited)){
            return true;
        }
        
    }
    if(node != start)
        visited.pop_back();
    return false;
}

bool Board::DFSPathExists(int currentNode, int goalNode, std::vector<int> &visited){
    if (goalNode == currentNode)
        return true;
    visited.push_back(currentNode);
    for(size_t i = 0; i < (*m_currentGraphPtr)[currentNode].size(); i++){
        
        if(vectorHelper_Exists( (*m_currentGraphPtr)[currentNode][i], visited))
            continue;

        if(DFSPathExists( (*m_currentGraphPtr)[currentNode][i], goalNode, visited))
            return true;

    }
    visited.pop_back();
    return false;
}

void Board::PerformVictory(){
    sf::Text text;
    if (m_currentPlayer == Player::WHITE){
        text = sf::Text("White Won!", m_font);
        text.setFillColor(sf::Color::White);
    } else {
        text = sf::Text("Red Won!", m_font);
        text.setFillColor(sf::Color::Red);
    }
    text.setCharacterSize(50);
    text.setStyle(sf::Text::Bold);
    // Draw it
    m_text.push_back(text); 

}

bool Board::checkWinningCondition(){
    //First check if there's a loop in the graph
    for (size_t i = 0; i < (*m_currentGraphPtr).size(); i++){
        if ((*m_currentGraphPtr)[i].size() < 2) 
            continue;
        std::vector<int> visited;
        if(DFSCycles(i, i, visited)){
            return true;
        }
    }

    //Now we have to differentiate between RED and WHITE because of the 
    //different numbering regarding the goals
    if (m_currentPlayer == Player::WHITE){

        for(int i = 0; i < 5; i++){
            for(int j = 0; j < 5; j++){
                std::vector<int> visited;
                if(DFSPathExists(i, 29-j, visited)){
                    /*sf::CircleShape tmp(20.0f, 6);
                    m_debugCircles.push_back(tmp);*/
                    return true;

                }
            }
        }

    } else {
        for(int i = 0; i < 25; i += 6){
            for(int j = 5; j < 30; j += 6){
                std::vector<int> visited;
                if(DFSPathExists(i, j, visited)){
                    return true;

                }
            }
        }
    
    }
    return false;
}

Board::Board(Board &board, bool test){
    if (test){
        this->m_whiteSquare_Graph = board.m_whiteSquare_Graph;
    }
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




//-------------AI STUFF----------------

int Board::BoardEvaluation(Board &tmpBoard){

    int result = 0;
    
    if (checkWinningCondition())
        result = 10000;

    if (m_currentPlayer == Player::WHITE){

    } else {
        
    }

    return result;
}

Board::Position Board::BestMove(Board &tmpBoard){

    //std::vector< std::vector <int> > tmpAIGraph = (*m_currentGraphPtr); 
    return {0,0};
}

int Board::pathHeuristic(Board &tmpBoard, std::vector<int> &visited){
    int result = 0;
    
    //visited.push_back(currentNode);
    for(size_t i = 0; i < (*m_currentGraphPtr).size(); i++){

        for(size_t j = 0; j < (*m_currentGraphPtr)[i].size(); j++){

        }
        
        /*if(vectorHelper_Exists( (*m_currentGraphPtr)[currentNode][i], visited))
            continue;

        if(DFSPathExists( (*m_currentGraphPtr)[currentNode][i], goalNode, visited))
            return true;*/

    }
    visited.pop_back();



    return result;
}

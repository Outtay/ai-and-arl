#include <SFML/Graphics.hpp>
#include "Board.h"
#include "AI.h"
#include <iostream>
#include <string>

//global variable as declared in Board.h
bool g_GAME_IS_WON = false;

int main(int argc, char *argv[])
{
    sf::RenderWindow window(sf::VideoMode(1000, 1000 ), "SFML works!");
    Board board(200, 200,800);
    //sf::CircleShape shape(100.f, 6);
    //shape.setFillColor(sf::Color::Green);
    
    bool hasPerformedVictory = false;

    bool playingWithAI = true;

    bool aiPlayingagainstItself = false;
    int depthHor = 3;
    bool aiBegins = false;
    bool alphaBetaPruningEnabled = true;


    //Parse command line arguments
    for (int i = 1; i < argc; i++){
		if (std::string(argv[i]).compare("-aibattle") == 0){
			aiPlayingagainstItself = true;
		}
		else if (std::string(argv[i]).compare("-hor") == 0){
            if (std::string(argv[i+1]).compare("inf") == 0){
                    depthHor = 100000;
                    i++;
                }
            else 
                depthHor = atoi(argv[++i]);
		}
		else if (std::string(argv[i]).compare("-aibegins") == 0){
			aiBegins = true;
		}
        else if (std::string(argv[i]).compare("-alphabetaDisable") == 0){
			alphaBetaPruningEnabled = false;
		}

	}


    AI ai(board, alphaBetaPruningEnabled, depthHor);
    

    if (aiBegins){
        board.removeLastVirtualConnection();
        board.SwitchPlayers(board.m_otherPlayer);
        Board::Position move = ai.dummyAIcall();
        board.CommitMoveAI(move);

        aiBegins = false;
    } else if (aiPlayingagainstItself){
        board.removeLastVirtualConnection();
    }


    while (window.isOpen())
   {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed){
                window.close();
            }
            if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Escape 
                        || event.key.code == sf::Keyboard::Q)){
                window.close();
            }

            if (!hasPerformedVictory && !aiPlayingagainstItself){

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right){
                    board.ShowMove(Board::Movement::RIGHT);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left){
                    board.ShowMove(Board::Movement::LEFT);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up){
                    board.ShowMove(Board::Movement::UP);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down){
                    board.ShowMove(Board::Movement::DOWN);
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return){
                    board.CommitMove();
                    if (playingWithAI && !g_GAME_IS_WON){
                        Board::Position move = ai.dummyAIcall();
                        board.CommitMoveAI(move);
                    }

                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space){
                    board.toggleMode();
                }
            } else if (aiPlayingagainstItself && !hasPerformedVictory){
                
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return){

                    if( !g_GAME_IS_WON){
                        Board::Position move = ai.dummyAIcall();
                        board.CommitMoveAIToAI(move);
                    }
                }

            } else {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return){
                    window.close();
                }

            }

        }

        if (g_GAME_IS_WON && !hasPerformedVictory){
            board.PerformVictory();
            hasPerformedVictory = true;
        }
        
        window.clear();
        board.RenderBoard(window);
        window.display();
    }

    return 0;
}

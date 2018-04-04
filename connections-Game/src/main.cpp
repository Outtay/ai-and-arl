#include <SFML/Graphics.hpp>
#include "Board.h"
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1000, 1000 ), "SFML works!");
    Board board(200, 200,800);
    //sf::CircleShape shape(100.f, 6);
    //shape.setFillColor(sf::Color::Green);

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

            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space){
                board.toggleMode();
            }

        }
        
        window.clear();
        board.RenderBoard(window);
        window.display();
    }

    return 0;
}

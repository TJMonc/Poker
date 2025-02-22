#include "Game.h"
#include <format>

int main(){
    Poker::PokerGame* game = new Poker::PokerGame;
    RenderWindow* window = game->getWindow();

    try{
        
        game->init();
        game->update();
    }
    catch(std::exception &e){
        sf::Text errorMsg;
        std::string eString = e.what();
        window->clear();
        errorMsg.setCharacterSize(100);
        errorMsg.setFillColor(Color::Red);
        errorMsg.setPosition((Vector2f)window->getSize() / 2.f);
        errorMsg.setString(std::format("Error: {}", eString));
        window->draw(errorMsg);
        window->display();
        std::cerr << e.what();
    }
    delete game;
}
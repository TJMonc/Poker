#include "Game.h"
#include <format>
#ifdef __unix__
int main(int argc, char** argv){

#endif

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, int nCmdShow){

#endif
    srand(time(NULL));
    Poker::PokerGame* game = new Poker::PokerGame;
    RenderWindow* window = game->getWindow();

    try{
        
        game->init();
        game->update();
    }
    catch(std::exception &e){
        sf::Font font;
        sf::Text errorMsg(font);

        Game::loadFont(errorMsg, font, Game::FontPaths::blackLivesFont);
        std::string eString = e.what();
        window->clear();
        errorMsg.setCharacterSize(100);
        errorMsg.setFillColor(Color::Red);
        errorMsg.setPosition((Vector2f)window->getSize() / 2.f);
        errorMsg.setString(std::format("Error: {}", eString));
        window->draw(errorMsg);
        window->display();
        std::cout << e.what();
    }
    delete game;

    return 0;
}
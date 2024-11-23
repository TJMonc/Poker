#include "Game.h"


int main(){
    RenderWindow window(VideoMode(1024,720), "Multiplayer Poker", Style::Resize);
    Poker::PokerGame* game = new Poker::PokerGame(window);
    game->init(window);
    game->update(window);

    delete game;
}
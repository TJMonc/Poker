#include "Game.h"


int main(){
    Poker::PokerGame* game = new Poker::PokerGame;
    game->init();
    game->update();

    delete game;
}
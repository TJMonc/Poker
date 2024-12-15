#include "Game.h"

typedef Poker::PokerGame::PlayerStruct player;
typedef Poker::PokerGame::GameState gamestate;
typedef Poker::Deck Deck;
typedef Poker::Hand Hand;

struct initPacket{
    int index;
    std::pair<int, int> cards[5];
};
struct packet1{
    int phase;
    bool isRaising;
    int raiseAmount;

};

struct packet3{
    int index;
    std::pair<int, int> cards[4][5];
};

struct packet2{
    std::pair<int, int> cards[5];
};




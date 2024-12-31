#include "Game.h"

typedef Poker::PokerGame::PlayerStruct player;
typedef Poker::PokerGame::GameState gamestate;
typedef Poker::Deck Deck;
typedef Poker::Hand Hand;


struct packet1{
    int phase;
    bool isRaising;
    int raiseAmount;
    
};

struct initPacket{
    int index;
    std::pair<int, int> cards[4][5];
};

struct packet2{
    int index;
    int discardNum;
    std::vector<int> discarded;
    std::pair<int, int> cards[5];
};








#include "Game.h"

typedef Poker::PokerGame::PlayerStruct player;
typedef Poker::PokerGame::GameState gamestate;
typedef Poker::Deck Deck;
typedef Poker::Hand Hand;


struct packet1{
    int phase;
    bool isRaising;
    int raiseAmount;
	bool folded = false;

};

struct initPacket{
    int index;
    std::pair<int, int> cards[4][5];
	int playerNum;
};

struct packet2{
	std::pair<int, int> cards[5];

    int index;
    int discardNum;
    int discarded[5];
	bool folded = false;
};
struct packet3{
    std::pair<int, int> cards[4][5];
};
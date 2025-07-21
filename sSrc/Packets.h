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
	/*1st index is player index. Second index is card. Card[0][3] would be player 1's third card
	 Second in pair is card suit. Refer to Poker::CardNumbers::Numbers and Poker::Suits::Suit enums
	*/
    std::pair<int, int> cards[4][5];
	int playerNum;
};

struct packet2{
	//First in the pair is card number. Second in pair is card suit. Refer to Poker::CardNumbers::Numbers and Poker::Suits::Suit enums
	std::pair<int, int> cards[5];

    int index;
    int discardNum;
    int discarded[5];
	bool folded = false;
};
struct packet3{
	/*1st index is player index. Second index is card. Card[0][3] would be player 1's third card
	First in the pair is card number. Second in pair is card suit. Refer to Poker::CardNumbers::Numbers and Poker::Suits::Suit enums
	*/
    std::pair<int, int> cards[4][5];
};
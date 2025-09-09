#ifndef POKERGAME_H
#define POKERGAME_H
#include <format>
#include "Hand.h"


#ifdef _WIN32

#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Windows.h"

#endif

#ifdef __unix__

#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h>
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

typedef int SOCKET;
typedef sockaddr_in SOCKADDR_IN;
typedef sockaddr SOCKADDR;
#endif


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

namespace Poker {
	class PokerGame {
	public:
		struct PlayerStruct {
			Hand playerHand;
			bool isPlayer = false;
			bool isRaising;
			int betMoney;
			int betAmount;
			bool bust;

			Text t_handType = Text(UIStruct::font);
			Text t_betMoney = Text(UIStruct::font);
			Text t_betAmount = Text(UIStruct::font);
		};
		struct GameState{
			int turn;
			int phase;
			int winnerIndex;
			int callAmount;
			int betPool;
			bool end;
			bool enterPressed = false;
			Clock interactionClock;
			Time interactionTime;

		};
		
		struct UIStruct{
			static Font font;

			Text callText = Text(font);
			RectangleShape callBox;
			const std::string callString = "Call";
			const std::string raiseString = "Raise";

			Text foldText = Text(font);
			RectangleShape foldBox;

			RectangleShape inputRect;
			bool foldPressed;
			bool isWriting;
			Text inputText = Text(font);
			std::string input;

			Text t_callAmount = Text(font);
			Text t_betPool = Text(font);


		};


		private:
			Vector2f windowScale;
			Deck deck;
			CircleShape mouseCircle;

			PlayerStruct players[4];
			GameState info;
			UIStruct display;
			initPacket initPack;
			sockaddr_in serverInfo;
			int you;

			CircleShape turnPointer;

			std::thread recieve;
			int threadProgress = 0;
		
		private:
			void initDeck(RenderWindow& window);
			void initPlayers(RenderWindow& window);
			void initGameState(RenderWindow& window);
			void initUI(RenderWindow& window);

			void betPhase(SOCKET* acceptSock);
			void discardPhase(SOCKET* acceptSock);
			void endPhase(SOCKET* acceptSock);
			void phaseChange();
			void displayInteraction(std::optional<Event>& event);
			void handleBetInput(const Event& e);

			void draw(RenderWindow& window);
			int recvThread(SOCKET *acceptSock, int *threadActive);

		public:
			PokerGame(RenderWindow& window) {
				windowScale = {Vector2f(window.getSize()) / Vector2f RES_768};
				turnPointer.setRadius(10.f * windowScale.x);
				turnPointer.setPointCount(3);
				turnPointer.setFillColor(Color::Red);
				deck.setWindow(&window);
				mouseCircle.setRadius(5.f);

			}
			void init(RenderWindow& window, SOCKET* acceptSock, sockaddr_in aServInfo);
			void update(RenderWindow& window, SOCKET* acceptSock);

	};
}

#endif
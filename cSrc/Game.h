#ifndef POKERGAME_H
#define POKERGAME_H
#include <format>
#include <mutex>
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

struct packet2 {
	//First in the pair is card number. Second in pair is card suit. Refer to Poker::CardNumbers::Numbers and Poker::Suits::Suit enums
	std::pair<int, int> cards[5];

    int index;
    int discardNum;
    int discarded[5];
	bool folded = false;
};
struct packet3 {
	/*1st index is player index. Second index is card. Card[0][3] would be player 1's third card
	First in the pair is card number. Second in pair is card suit. Refer to Poker::CardNumbers::Numbers and Poker::Suits::Suit enums
	*/
    std::pair<int, int> cards[4][5];
};

struct DisconnectPacket {
	int index = -1;
	int phase = -1;
};

struct PacketHeader {
	bool isDisconnectedPeer = false;
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
			std::atomic<int> threadProgress = 0;
			std::atomic<bool> running = true;
			std::mutex mutex;

			std::unique_ptr<packet1> packPtr1 = nullptr;
			std::unique_ptr<packet2> packPtr2 = nullptr;
			std::unique_ptr<packet3> packPtr3 = nullptr;





	
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
			int applyPacket1(std::unique_ptr<packet1> &pack) {

				if (pack->folded) {
					players[info.turn].playerHand.setFolded(true);
				}
				else {
					int raiseAmount = pack->raiseAmount;
					bool isRaising = pack->isRaising;
					if (pack->isRaising) {
						int diff = raiseAmount + (info.callAmount - players[info.turn].betAmount);
						if (diff > players[info.turn].betMoney) {
							diff = players[info.turn].betMoney;
							raiseAmount = diff - info.callAmount;
						}
						players[info.turn].betMoney -= diff;
						info.betPool += diff;
						info.callAmount += raiseAmount;
						players[info.turn].betAmount += diff;
					}
					else {
						players[info.turn].isRaising = false;
						if (players[info.turn].betAmount < info.callAmount) {
							if (players[info.turn].betMoney < (info.callAmount - players[info.turn].betAmount)) {
								players[info.turn].betAmount += players[info.turn].betMoney;
								players[info.turn].betMoney = 0;
							}
							else {
								int diff = info.callAmount - players[info.turn].betAmount;
								players[info.turn].betMoney -= diff;
								info.betPool += diff;

								players[info.turn].betAmount = info.callAmount;
							}
						}
					}
				}
				players[info.turn].t_betAmount.setString(std::to_string(players[info.turn].betAmount));
				players[info.turn].t_betMoney.setString(std::to_string(players[info.turn].betMoney));

				return 0;
			};
			int applyPacket2(std::unique_ptr<packet2>& pack2){
				for (int i = 0; i < 5; i++) {

					players[pack2->index].playerHand.pat(i) = &deck.at(std::format("{}{}", Suits::suit.at(pack2->cards[i].second), pack2->cards[i].first));
				}
				players[pack2->index].playerHand.sortCards();

				for (size_t i = 0; i < 4; i++) {
					if (!players[i].playerHand.getIsPlayer()) {
						players[i].playerHand.setTurned(true);
					}
				}
				return 0;
			};
			int applyPacket3(std::unique_ptr<packet3>& pack){
				deck.reset();

				players[info.winnerIndex].betMoney += info.betPool;
				info.winnerIndex = 0;
				info.betPool = 0;
				info.callAmount = 5;
				for (size_t i = 0; i < 4; i++) {
					auto &hand = players[i].playerHand;
					for (int j = 0; j < 5; j++)
					{
						players[i].playerHand[j].getSprite().setColor(Color::White);
					}
				}

				for (int i = 0; i < 4; i++) {
					auto &hand = players[i].playerHand;

					players[i].t_handType.setFillColor(Color::Blue);
					players[i].t_handType.setString(Poker::Hand::typesMap.at(hand.getHandType()));
					players[i].t_betMoney.setString(std::to_string(players[i].betMoney));
					if (!hand.getIsPlayer()) {
						hand.setTurned(true);
					}
					if (players[i].betMoney < 1) {
						players[i].bust = true;
					}
					for (int j = 0; j < 5; j++) {
						hand.pat(j) = &deck.at(std::format("{}{}",
														   Suits::suit.at(pack->cards[i][j].second), pack->cards[i][j].first));
					}
					for (size_t i = 0; i < 4; i++) {
						if (!players[i].playerHand.getIsPlayer()) {
							players[i].playerHand.setTurned(true);
						}
					}
					hand.sortCards();
				}
				info.end = false;
				return 0;
			};
			int applyDisconnect(std::unique_ptr<DisconnectPacket>& packet);
			int recvThread(SOCKET *acceptSock, void* packet);

			

			bool verifyConnections(SOCKET* acceptSock);

		public:
			PokerGame(RenderWindow& window) {
				windowScale = {Vector2f(window.getSize()) / Vector2f RES_768};
				turnPointer.setRadius(10.f * windowScale.x);
				turnPointer.setPointCount(3);
				turnPointer.setFillColor(Color::Red);
				deck.setWindow(&window);
				mouseCircle.setRadius(5.f);

			}
			~PokerGame(){
				running = false;

				if (recieve.joinable()){
					recieve.join();
				}
			}
			void init(RenderWindow& window, SOCKET* acceptSock, sockaddr_in aServInfo);
			void update(RenderWindow& window, SOCKET* acceptSock);

	};
}

#endif
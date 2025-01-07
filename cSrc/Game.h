#ifndef POKERGAME_H
#define POKERGAME_H
#include <format>
#include "Hand.h"
#include <WinSock2.h>
#include <WS2tcpip.h>


struct packet1{
    int phase;
    bool isRaising;
    int raiseAmount;

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

			Text t_handType;
			Text t_betMoney;
		};
		struct GameState{
			int turn;
			int phase;
			int winnerIndex;
			int callAmount;
			int betPool;
			bool end;
			Clock interactionClock;
			Time interactionTime;

		};
		
		struct UIStruct{
			Font font;

			Text callText;
			RectangleShape callBox;
			const std::string callString = "Call";
			const std::string raiseString = "Raise";

			Text foldText;
			RectangleShape foldBox;

			RectangleShape inputRect;
			bool foldPressed;
			bool isWriting;
			Text inputText;
			std::string input;
			Text t_callAmount;

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
			void displayInteraction(Event& anEvent);
			void draw(RenderWindow& window);

			int recvThread(SOCKET *acceptSock, int *threadActive) {
				using namespace std::chrono_literals;
				this->threadProgress = 1;

				switch (info.phase) {
				case 0: {
					packet1 pack;
					int addrSize = sizeof(serverInfo);
					int bytes = 0;
					if ((bytes = recv(*acceptSock, (char *)&pack, sizeof(packet1), 0)) > 0) {
						int raiseAmount = pack.raiseAmount;
						bool isRaising = pack.isRaising;

						if (pack.isRaising) {

							int diff = raiseAmount + (info.callAmount - players[info.turn].betAmount);
							if (diff > players[info.turn].betMoney) {
								diff = players[info.turn].betMoney;
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
						players[info.turn].t_betMoney.setString(std::to_string(players[info.turn].betMoney));
					}
					else{
						std::cout << WSAGetLastError();
					}
					break;

				}

				case 1:{
					try{
					packet2 pack2;
					int recvCount;
					if((recvCount = recv(*acceptSock, (char *)&pack2, sizeof(packet2), 0)) != SOCKET_ERROR){
						for (int i = 0; i < 5; i++) {

							std::cout << "index: " << i;
							std::cout << "\nvalue: " << recvCount;
							players[info.turn].playerHand.pat(i) = &deck.at(std::format("{}{}",
																	Suits::suit.at(pack2.cards[i].second), pack2.cards[i].first));
						}
						players[info.turn].playerHand.sortCards();
					}
					}
					catch(std::out_of_range& e){
						std::cout << e.what();
					}

				}
				break;

				}
				this->threadProgress = 2;
				return 0;
			}

		public:
			PokerGame(RenderWindow& window) {
				windowScale = {Vector2f(window.getSize()) / Vector2f RES_768};
				deck.setWindow(&window);
				mouseCircle.setRadius(5.f);

			}
			void init(RenderWindow& window, SOCKET* acceptSock, sockaddr_in aServInfo);
			void update(RenderWindow& window, SOCKET* acceptSock);

	};
}

#endif
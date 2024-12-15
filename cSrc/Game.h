#ifndef POKERGAME_H
#define POKERGAME_H
#include <format>
#include "Hand.h"
#include <WinSock2.h>
#include <WS2tcpip.h>


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

namespace Poker {
	class PokerGame {
	public:
		struct PlayerStruct {
			Hand playerHand;
			bool isPlayer;
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
			packet3 initPack;
			int you;
		
		private:
			void initDeck(RenderWindow& window);
			void initPlayers(RenderWindow& window);
			void initGameState(RenderWindow& window);
			void initUI(RenderWindow& window);

			void betPhase();
			void discardPhase();
			void endPhase();
			void phaseChange();
			void displayInteraction(Event& anEvent);
			void draw(RenderWindow& window);
		public:
			PokerGame(RenderWindow& window) {
				windowScale = {Vector2f(window.getSize()) / Vector2f RES_768};
				deck.setWindow(&window);
				mouseCircle.setRadius(5.f);

			}
			void init(RenderWindow& window, SOCKET* acceptSock);
			void update(RenderWindow& window, SOCKET* acceptSock);

	};
}

#endif
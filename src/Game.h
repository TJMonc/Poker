#ifndef POKERGAME_H
#define POKERGAME_H
#include "Hand.h"

namespace Poker {
	class PokerGame {
	private:
		struct PlayerStruct {
			Hand playerHand;
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
			Clock interactionClock;
			Time interactionTime;

		};
		
		struct UIStruct{
			Font font;

			Text callText;
			RectangleShape callBox;
			std::string callString;
			std::string raiseString;

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
			RenderWindow window;
			Vector2f windowScale;
			Deck deck;
			CircleShape mouseCircle;

			PlayerStruct players[4];
			GameState info;
			UIStruct display;

		
		private:
			void init();
			void initDeck();
			void initPlayers();
			void initGameState();
			void initUI();
	};
}

#endif
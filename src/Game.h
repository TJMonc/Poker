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
			RenderWindow window;
			Vector2f windowScale;
			Deck deck;
			CircleShape mouseCircle;

			PlayerStruct players[4];
			GameState info;
			UIStruct display;
		
		private:
			int log = 0;

			void initDeck();
			void initPlayers();
			void initGameState();
			void initUI();

			void betPhase();
			void discardPhase();
			void endPhase();
			void phaseChange();
			void displayInteraction();
			void handleBetInput(const Event& e);
		void draw();
		public:
			PokerGame(): window(VideoMode(), "Poker", State::Fullscreen){
				windowScale = {Vector2f(window.getSize()) / Vector2f RES_768};
				deck.setWindow(&window);
				mouseCircle.setRadius(5.f);

			}
			void init();
			void update();

			RenderWindow* getWindow(){
				return &window;
			}

	};
}

#endif
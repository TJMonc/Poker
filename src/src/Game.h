#ifndef POKERGAME_H
#define POKERGAME_H
#include "Hand.h"

namespace Poker {
	class PokerGame {
	private:
		std::unique_ptr<RenderWindow> window = std::make_unique<RenderWindow>(sf::VideoMode(), "SFML Practice", sf::Style::Fullscreen);
		CircleShape mouseCircle;
		Vector2f windowScale;
		int turnCounter;
		int phases = 0;
		int potMoney = 0;
		int callAmount = 0;

		Clock interactionClock;
		Time interactionTime = milliseconds(200);

	public:
		struct PlayerStruct {
			Hand hand;
			int money;
			bool isTurn;
			bool isPlayer = false;
			bool call = false;
			bool raise = false;

		};
		struct UIStruct {
			Font font;
			Text inputText;
			std::string playerInput;
			RectangleShape textBox;

			RectangleShape betTypeBox;
			Text callOrRaise;
			Text handTypeText;

			bool isWriting = false;
			bool call = true;
		};

	private:
		std::array<PlayerStruct, 4> players;
		std::unique_ptr<Deck> deck;
		std::unique_ptr<UIStruct> userInterface;

	public:
		void init();
		void initPlayers();
		void initUI();
	};
}

#endif
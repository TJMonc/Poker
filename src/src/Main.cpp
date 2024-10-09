#include "Game.h"

//TODO: Finish mouse if statement.
int main() {
	RenderWindow window(sf::VideoMode(), "SFML Practice", sf::Style::Fullscreen);
	Vector2f windowScale = Vector2f(window.getSize()) / Vector2f RES_768;
	CircleShape mouseCircle(5.f);
	Poker::Deck deck;
	Vector2f deckPos = {
	window.getSize().x - deck[0].getGlobalBounds().getSize().x,
	window.getSize().y - deck[0].getGlobalBounds().getSize().y
	};
	deckPos = (1.f / 2.f) * deckPos;
	deck.setPosition(deckPos);


	Poker::Hand* players = new Poker::Hand[4];
	for(size_t i = 0; i < 4; i++) {
		players[i].setDeck(&deck);
		players[i].setWindow(&window);
	}
    Vector2f mid = {
        (window.getSize().x / 2.f) - (players[0].getSize().x / 2.f),
       (window.getSize().y / 2.f - players[0].getSize().y / 2.f)
        
    };
	Vector2f neg = {
		(window.getSize().x - players[0].getSize().x),
		(window.getSize().y - players[0].getSize().y)
	};
	players[0].setPosition({mid.x, neg.y});
	players[1].setPosition({0.f, mid.y});
	players[2].setPosition({mid.x, 0.f});
	players[3].setPosition({neg.x, mid.y});

	players[0].setIsPlayer(true);


	RectangleShape inputTextRect;
	inputTextRect.setOutlineColor(Color::Blue);
	inputTextRect.setOutlineThickness(10.f * windowScale.y);
	inputTextRect.setSize(Vector2f(200.f, 50.f) * windowScale);
	inputTextRect.setPosition({50 * windowScale.x, window.getSize().y - inputTextRect.getSize().y - 50 * windowScale.y});
	bool isRaising[4] = {false, false, false, false};	//This should be false

	Font font;
	font.loadFromFile(Game::FontPaths::blackLivesFont);

	Text inputText;
	inputText.setFont(font);
	inputText.setCharacterSize(40.f * windowScale.x);
	inputText.setFillColor(Color::Black);
	inputText.setPosition(inputTextRect.getPosition());
	std::string input = "";
	bool isWriting = false;

	RectangleShape callBox(Vector2f(100.f, 50.f) * windowScale);
	callBox.setOutlineColor(Color::Blue);
	callBox.setOutlineThickness(5.f * windowScale.y);
	callBox.setPosition(
		window.getSize().x - callBox.getSize().x - 50.f * windowScale.x,
		window.getSize().y - callBox.getSize().y - 50.f * windowScale.y
	);
	Text callText;
	std::string callString = "Call";
	std::string raiseString = "Raise";
	callText.setFont(font);
	callText.setCharacterSize(30.f * windowScale.x);
	callText.setPosition(callBox.getPosition());
	callText.setFillColor(Color::Blue);
	callText.setString(callString);

	bool betPhase = true;
	bool cardPhase = false;
	bool endPhase = false;
	int turn = 0;
	int lol = 0;
	int phase = 0;

	int betPool = 0;
	int betMoney[4] = {3000, 3000, 3000, 3000};
	int betAmount[4] = {0, 0, 0, 0};
	int callAmount = 0;

	for(size_t i = 0; i < 4; i++){
		if (!players[i].getIsPlayer()) {
			players[i].setTurned(true);
		}
	}

	Clock interactionClock;
	Time interactionTime = milliseconds(200);
	while (window.isOpen()) {
		Event anEvent;
		while (window.pollEvent(anEvent)) {
			if (Event::Closed == anEvent.type) {
				window.close();
			}
		}

		mouseCircle.setPosition(Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y));
		lol++;
		players[turn].updateMouse(mouseCircle);

		switch (phase) {
		case 0:
			if (players[turn].getIsPlayer()) {
				if (Keyboard::isKeyPressed(Keyboard::Enter) && interactionClock.getElapsedTime() > interactionTime) {
					interactionClock.restart();
					if (isRaising[turn]) {
						int raiseAmount = std::stoi(input);
						int diff = raiseAmount + (callAmount - betAmount[turn]);
						betMoney[turn] -= diff;
						betPool += diff;
						callAmount += raiseAmount;
						betAmount[turn] += diff;
					}
					else {
						if (betAmount[turn] < callAmount) {
							if (betMoney[turn] < (callAmount - betMoney[turn])) {
								betAmount[turn] += betMoney[turn];
								betMoney[turn] = 0;
							}
							else {
								int diff = callAmount - betAmount[turn];
								betMoney[turn] -= diff;
								betPool += diff;

								betAmount[turn] = callAmount;
							}
						}
					}
					turn++;

				}
			}
			else {
				isRaising[turn] = rand() % 2;
				if (isRaising[turn]) {
					int raiseAmount = rand() % (callAmount - betAmount[turn] + betMoney[turn]);
					int diff = raiseAmount + (callAmount - betAmount[turn]);
					betMoney[turn] -= diff;
					betPool += diff;
					callAmount += raiseAmount;
					betAmount[turn] += diff;
				}
				else {
					int diff = callAmount - betAmount[turn];
					betMoney[turn] -= diff;
					betPool += diff;

					betAmount[turn] = callAmount;
					players[turn].hasChosen = false;
				}
				turn++;
			}
			break;
		case 1:
			if (players[turn].getIsPlayer()) {
				if (Keyboard::isKeyPressed(Keyboard::Enter) && interactionClock.getElapsedTime() > interactionTime) {
					interactionClock.restart();
					players[turn].discardCards();
					players[turn].setHandType();
					turn++;
				}
			}
			else {
				players[turn].discardCards();
				players[turn].setHandType();
				players[turn].setTurned(true);
				turn++;
			}
		}
		if (turn > 3) {
			turn = 0;
			phase++;
		}
		if (phase > 2) {
			phase = 0;
			turn = 0;
		}

		if (betPhase && turn == 0) {

			if (Mouse::isButtonPressed(Mouse::Left) && lol > 30) {
				lol = 0;
				if (mouseCircle.getGlobalBounds().intersects(callBox.getGlobalBounds())) {
					if (callText.getString() == callString) {
						isRaising[0] = true;
						callText.setString(raiseString);
					}
					else {
						isRaising[0] = false;
						callText.setString(callString);
					}
				}
				if (mouseCircle.getGlobalBounds().intersects(inputTextRect.getGlobalBounds())) {
					isWriting = true;
				}
				else {
					isWriting = false;
				}
			}
			if (isWriting && isRaising) {
				std::string validNums = "1234567890";

				if (anEvent.type == Event::TextEntered && lol > 10) {
					lol = 0;
					if (anEvent.text.unicode == '\b') {
						if (inputText.getString() != "") {
							input.erase(input.size() - 1);
						}
					}
					else {
						input += anEvent.text.unicode;

						if (input.find_first_of(validNums) == std::string::npos) {
							input.erase(input.size() - 1);
						}
					}
				}
				inputText.setString(input);
			}
			else if (phase != 0) {
				input = "";
				inputText.setString(input);

			}
		}
		window.clear();
		for (size_t i = 0; i < 4; i++) {
			players[i].drawTo(window);

		}
		deck.drawTo(window);
		if (phase == 0 || phase == 2) {
			window.draw(callBox);
			window.draw(callText);
			if (isRaising[0])
			{
				window.draw(inputTextRect);
				window.draw(inputText);
			}
			if (turn > 3) {
				turn = 0;
			}

			mouseCircle.setPosition(Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y));
			lol++;
			if (Keyboard::isKeyPressed(Keyboard::Enter) && lol > 30) {
				lol = 0;
				if (cardPhase) {
					for (size_t i = 0; i < 4; i++) {
						players[i].discardCards();
						players[i].setHandType();

						std::cout
							<< "Hand " << i + 1 << std::endl
							<< players[i];
					}
				}

			}

			if (players[turn].getIsPlayer()) {
				if (Keyboard::isKeyPressed(Keyboard::Enter) && interactionClock.getElapsedTime() > interactionTime) {
					interactionClock.restart();
					if (isRaising[turn]) {
						int raiseAmount = std::stoi((std::string)inputText.getString());
					}
				}
			}

			players[turn].updateMouse(mouseCircle);
			if (betPhase && turn == 0) {

				if (Mouse::isButtonPressed(Mouse::Left) && lol > 30) {
					lol = 0;
					if (mouseCircle.getGlobalBounds().intersects(callBox.getGlobalBounds())) {
						if (callText.getString() == callString) {
							isRaising[0] = true;
							callText.setString(raiseString);
						}
						else {
							isRaising[0] = false;
							callText.setString(callString);
						}
					}
					if (mouseCircle.getGlobalBounds().intersects(inputTextRect.getGlobalBounds())) {
						isWriting = true;
					}
					else {
						isWriting = false;
					}
				}
				if (isWriting && isRaising) {
					if (anEvent.type == Event::TextEntered && lol > 10) {
						lol = 0;
						if (anEvent.text.unicode == '\b') {
							input.erase(input.size() - 1);
						}
						else {
							input += anEvent.text.unicode;
						}
					}
					inputText.setString(input);
				}
			}
			window.clear();
			for (size_t i = 0; i < 4; i++) {
				players[i].drawTo(window);
			}
			deck.drawTo(window);
			window.draw(callBox);
			window.draw(callText);
			if (isRaising[0]) {
				window.draw(inputTextRect);
				window.draw(inputText);
			}
			window.display();
		}
		delete[] players;
		return 0;
	}

}
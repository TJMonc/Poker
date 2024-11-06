
#include "Game.h"
int main() {
	srand(time(NULL));
	RenderWindow window(sf::VideoMode(), "SFML Practice", sf::Style::Fullscreen);
	window.setFramerateLimit(60);
	Vector2f windowScale = Vector2f(window.getSize()) / Vector2f RES_768;
	CircleShape mouseCircle(5.f);
	Poker::Deck deck;
	deck.setWindow(&window);

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
	players[0].setPosition({mid.x, neg.y - 30.f * windowScale.y});
	players[1].setPosition({windowScale.x, mid.y});
	players[2].setPosition({mid.x, 30.f * windowScale.y});
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

	RectangleShape foldBox(callBox.getSize());
	foldBox.setOutlineColor(Color::Red);
	foldBox.setOutlineThickness(5.f * windowScale.y);
	foldBox.setPosition(callBox.getPosition().x, callBox.getPosition().y - callBox.getSize().y - 30.f * windowScale.y);
	Text foldText;
	foldText.setFont(font);
	foldText.setCharacterSize(30.f * windowScale.x);
	foldText.setFillColor(Color::Red);
	foldText.setPosition(foldBox.getPosition());
	foldText.setString("FOLD");
	bool foldPressed = false;

	bool endPhase = false;
	int turn = 0;
	int lol = 0;
	int phase = 0;

	int betPool = 0;
	int betMoney[4] = {3000, 3000, 3000, 3000};
	bool called[4] = {false, false, false, false};
	std::string str_betMoney[4];
	Text text_betMoney[4];
	for(size_t i = 0; i < 4; i++){
		str_betMoney[i] = std::to_string(betMoney[i]);
		text_betMoney[i].setFont(font);
		text_betMoney[i].setCharacterSize(20.f * windowScale.x);
		text_betMoney[i].setPosition(players[i].getPosition().x, players[i].getPosition().y - 20 * windowScale.y);
		text_betMoney[i].setFillColor(Color::Cyan);
		text_betMoney[i].setString(std::to_string(betMoney[i]));
	}

	int betAmount[4] = {0, 0, 0, 0};
	int callAmount = 0;

	Text text_handType[4];
	for(size_t i = 0; i < 4; i++){
		text_handType[i].setFont(font);
		text_handType[i].setCharacterSize(20.f * windowScale.x);
		text_handType[i].setPosition(players[i].getPosition().x, players[i].getPosition().y + players[i].getSize().y);
		text_handType[i].setFillColor(Color::Blue);
		text_handType[i].setString(Poker::Hand::typesMap.at(players[i].getHandType()));
	}

	Text t_callAmount;
	t_callAmount.setFont(font);
	t_callAmount.setCharacterSize(20.f * windowScale.x);
	t_callAmount.setFillColor(Color::White);
	t_callAmount.setPosition(deck.getPositon().x, deck.getPositon().y - 50.f * windowScale.y);
	t_callAmount.setString(std::to_string(callAmount));
	for(size_t i = 0; i < 4; i++){
		if (!players[i].getIsPlayer()) {
			players[i].setTurned(true);
		}
	}
	Clock interactionClock;
	Time interactionTime = milliseconds(200);
	int winnerIndex;
	bool bust[4] = {false, false, false, false};

	auto bet = [&]() {
		if(players[turn].getFolded()){
			turn++;
		}
		else if (players[turn].getIsPlayer()) {
			if ((Keyboard::isKeyPressed(Keyboard::Enter)) && interactionClock.getElapsedTime() > interactionTime) {
				interactionClock.restart();
				if(foldPressed){
					players[turn].setFolded(true);
				}
				else if (isRaising[turn] && input != "") {
					int raiseAmount = std::stoi(input);
					int diff = raiseAmount + (callAmount - betAmount[turn]);
					if(diff > betMoney[turn]){
						diff = betMoney[turn];
					}
					betMoney[turn] -= diff;
					betPool += diff;
					callAmount += raiseAmount;
					betAmount[turn] += diff;
					called[turn] = false;

				}
				else {
					isRaising[turn] = false;
					if (betAmount[turn] < callAmount) {
						if (betMoney[turn] < (callAmount - betAmount[turn])) {
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
					called[turn] = true;
				}
				text_betMoney[turn].setString(std::to_string(betMoney[turn]));

				turn++;
			}
		}
		else {
			int randNum = std::rand() % 10 + 1;
			isRaising[turn] = (randNum > 8);
			if ((betMoney[turn] - callAmount) < 0) {
				isRaising[turn] = false;
			}
			if (isRaising[turn]) {
				int raiseAmount = rand() % (betMoney[turn] - callAmount);
				int diff = raiseAmount + (callAmount - betAmount[turn]);
				betMoney[turn] -= diff;
				betPool += diff;
				callAmount += raiseAmount;
				betAmount[turn] += diff;
				called[turn] = false;
			}
			else {
				if (betAmount[turn] < callAmount) {
					if (betMoney[turn] < (callAmount - betAmount[turn])) {
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
				called[turn] = true;
			}
			players[turn].hasChosen = false;
			text_betMoney[turn].setString(std::to_string(betMoney[turn]));
			turn++;
		}
	};

	while (window.isOpen()) {
		Event anEvent;
		while (window.pollEvent(anEvent)) {
			if (Event::Closed == anEvent.type) {	
				window.close();
			}
		}
		t_callAmount.setString(std::to_string(callAmount));

		if(bust[turn]){
			players[turn].setFolded(true);
		}
		mouseCircle.setPosition(Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y));
		lol++;
		players[turn].updateMouse(mouseCircle);

		switch (phase) {
		case 0:
			bet();
			break;
		case 1:
			if(players[turn].getFolded()){
				turn++;
			}
			else if(players[turn].getIsPlayer()){
				if(Keyboard::isKeyPressed(Keyboard::Enter) && interactionClock.getElapsedTime() > interactionTime){
					interactionClock.restart();
					players[turn].discardCards();
					players[turn].setHandType();
					text_handType[turn].setString(Poker::Hand::typesMap.at(players[turn].getHandType()));
					turn++;
				}
			}
			else{
				players[turn].discardCards();
				players[turn].setHandType();
				players[turn].setTurned(true);
				text_handType[turn].setString(Poker::Hand::typesMap.at(players[turn].getHandType()));
				players[turn].hasChosen = false;
				turn++;				
			}
			break;
		case 2:
			bet();
			break;
		case 3:
			Poker::Hand* winner = &players[0];
			if (!endPhase) {
				winnerIndex = 0;

				for (size_t i = 0; i < 4; i++) {
					if(players[i].getFolded()){
						if(winnerIndex == i){
							winnerIndex++;
							winner = &players[i + 1];
						}
						continue;
					}
					if (winner->getHandType() < players[i].getHandType()) {
						winner = &players[i];
						winnerIndex = i;
					}
					else if (winner->getHandType() == players[i].getHandType()) {
						if (winner->getHandType() == Poker::Hand::Flush) {
							if (players->getHighSuit() > winner->getHighSuit()) {
								winner = &players[i];
								winnerIndex = i;
							}
						}
						else if (players[i].getHighCard() > winner->getHighCard()) {
							winner = &players[i];
							winnerIndex = i;
						}
					}


				}
				for(size_t i = 0; i < 4; i++){
					if(i == winnerIndex){
						text_handType[i].setString("Winner: " + static_cast<std::string>(text_handType[i].getString()));
						text_handType[i].setFillColor(Color::Green);
					}
					else{
						text_handType[i].setFillColor(Color::Red);
					}
					players[i].setFolded(false);
					players[i].setTurned(false);
					betAmount[i] = 0;
				}
				endPhase = true;
				foldPressed = false;

			}
			if(Keyboard::isKeyPressed(Keyboard::Enter) && interactionClock.getElapsedTime() > interactionTime){
				interactionClock.restart();
				deck.reset();

				phase++;
				betMoney[winnerIndex] += betPool;
				winnerIndex = 0;
				betPool = 0;
				callAmount = 5;
				endPhase = false;
				for(size_t i = 0; i < 4; i++){
					for (int j = 0; j < 5; j++){
						players[i][j].getSprite().setColor(Color::White);
					}
					players[i].setDeck(&deck);
					players[i].setHandType();
					text_handType[i].setFillColor(Color::Blue);
					text_handType[i].setString(Poker::Hand::typesMap.at(players[i].getHandType()));
					text_betMoney[i].setString(std::to_string(betMoney[i]));
					if(!players[i].getIsPlayer()){
						players[i].setTurned(true);
					}
					if(betMoney[i] < 1){
						bust[i] = true;
					}
				}
			}

			//TODO FINISH THIS
			break;
		}
		//Turn Case ends

		if (turn > 3) {
			turn = 0;
			phase++;
			bool allCall;
			for (size_t i = 0; i < 4; i++) {
				if (players[i].getFolded()) {
					continue;
				}
				if (betAmount[i] < callAmount && betMoney[i] > 0) {
					allCall = false;
					break;
				}
				allCall = true;
			}
			if (!allCall) {
				phase--;
			}
		}
		if(phase > 3){
			phase = 0;
			turn = 0;
		}
		
		if ((turn == 0 || turn == 2)) {

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
				else if(mouseCircle.getGlobalBounds().intersects(foldBox.getGlobalBounds()) && !isRaising[0]){
					foldPressed = true;
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

				if (anEvent.type == Event::TextEntered && lol > 20)	{
					lol = 0;
					if (anEvent.text.unicode == '\b') {
						if (inputText.getString() != "") {
							input.erase(input.size() - 1);
						}
					}
					else {
						input += anEvent.text.unicode;
						
						if(input.find_first_of(validNums) == std::string::npos){
							input.erase(input.size() - 1);
						}
					}
				}
				inputText.setString(input);
			}
			else if(phase != 0){
				input = "";
				inputText.setString(input);

			}
		}
		window.clear();
		for(size_t i = 0; i < 4; i++) {
			players[i].drawTo(window);
			window.draw(text_betMoney[i]);
			if(players[i].getIsPlayer() || phase == 3){
				window.draw(text_handType[i]);
			}
		}
		deck.drawTo(window);
		if (phase == 0 || phase == 2) {
			window.draw(callBox);
			window.draw(callText);
			if (isRaising[0]){
				window.draw(inputTextRect);
				window.draw(inputText);
			}
			else{
				window.draw(foldBox);
				window.draw(foldText);
			}
		}
		window.draw(t_callAmount);
		window.display();
	}
	delete[] players;
	return 0;
}
#include "Game.h"

void Poker::PokerGame::init(RenderWindow& window, SOCKET* acceptSock, sockaddr_in aServInfo) {
	this->serverInfo = aServInfo;
	if(recv(*acceptSock, (char*)&initPack, sizeof(initPacket), 0) > 0){
		you = initPack.index;
	};
    initDeck(window);

    initGameState(window);

	initPlayers(window);

    initUI(window);
	draw(window);

}

void Poker::PokerGame::update(RenderWindow& window, SOCKET* clientSock) {
	sockaddr_in addrInfo = {0};
	int addrSize = sizeof(addrInfo);
	bool sent = false;
	getpeername(*clientSock, reinterpret_cast<SOCKADDR*>(&addrInfo), &addrSize);
	while(window.isOpen()){

		Event anEvent;
		while(window.pollEvent(anEvent)){

			switch(anEvent.type){
			case Event::EventType::Closed:
				window.close();
				break;
			}
			if(Keyboard::isKeyPressed(Keyboard::Escape)){
				window.close();
			}
		}

		display.t_callAmount.setString(std::to_string(info.callAmount));

		if (players[info.turn].bust) {
			std::cin.get();


			players[info.turn].playerHand.setFolded(true);
		}


		mouseCircle.setPosition(Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y));
		players[info.turn].playerHand.updateMouse(mouseCircle);
		if(!sent && info.turn == you){
			send(*clientSock, (char*)&info.phase, sizeof(int), 0);
			sent = true;
		}
		switch(info.phase){
			case 0:
				betPhase(clientSock);

				break;
			case 1:
				discardPhase(clientSock);
				break;
			case 2:
				betPhase(clientSock);
				break;
			case 3:
				endPhase(clientSock);
				break;
			
		}
		for(int i = 0; i < 4; i++){
			players[i].playerHand.setTurned(false);
		}
		phaseChange();
		if(info.turn != you){
			sent = false;
		}
		displayInteraction(anEvent);
		draw(window);
	}
}

void Poker::PokerGame::initDeck(RenderWindow& window) {
    Vector2f deckPos = {
        window.getSize().x - deck[0].getGlobalBounds().getSize().x,
        window.getSize().y - deck[0].getGlobalBounds().getSize().y
    };
    deckPos = (1.f / 2.f) * deckPos;
    deck.setPosition(deckPos);
}

void Poker::PokerGame::initPlayers(RenderWindow& window) {

    for (int i = 0; i < 4; i++) {
        players[i].playerHand.setDeck(&deck);
        players[i].playerHand.setWindow(&window);
        players[i].isRaising = false;
        players[i].betMoney = 5000;
        players[i].betAmount = 0;
        players[i].bust = false;
		players[i].isPlayer = true;
		for(int j = 0; j < 5; j++){
			players[i].playerHand.pat(j) = &deck.at(std::format("{}{}",
			 							Suits::suit.at(initPack.cards[i][j].second), initPack.cards[i][j].first));

		}
		players[i].playerHand.sortCards();

    }
	players[you].playerHand.setIsPlayer(true);

    Vector2f mid = {
        (window.getSize().x / 2.f) - (players[0].playerHand.getSize().x / 2.f),
        (window.getSize().y / 2.f - players[0].playerHand.getSize().y / 2.f)

    };
    Vector2f neg = {
        (window.getSize().x - players[0].playerHand.getSize().x),
        (window.getSize().y - players[0].playerHand.getSize().y)
    };

    players[0].playerHand.setPosition({mid.x, neg.y - 30.f * windowScale.y});
    players[1].playerHand.setPosition({windowScale.x, mid.y});
    players[2].playerHand.setPosition({mid.x, 30.f * windowScale.y});
    players[3].playerHand.setPosition({neg.x, mid.y});
    
    for(int i = 0; i < 4; i++){
        players[i].t_betMoney.setFont(display.font);
        players[i].t_betMoney.setCharacterSize(20.f * windowScale.x);
        players[i].t_betMoney.setPosition(players[i].playerHand.getPosition().x, players[i].playerHand.getPosition().y - 20 * windowScale.y);

        players[i].t_betMoney.setFillColor(Color::Cyan);
        players[i].t_betMoney.setString(std::to_string(players[i].betMoney));
    }

    for(size_t i = 0; i < 4; i++){
        Vector2f t_hTPos = {
            players[i].playerHand.getPosition().x,
            players[i].playerHand.getPosition().y + players[i].playerHand.getSize().y
        
        };
        players[i].t_handType.setFont(display.font);
		players[i].t_handType.setCharacterSize(20.f * windowScale.x);
		players[i].t_handType.setPosition(t_hTPos);
		players[i].t_handType.setFillColor(Color::Blue);
		players[i].t_handType.setString(Poker::Hand::typesMap.at(players[i].playerHand.getHandType()));
		if (!players[i].playerHand.getIsPlayer()) {
            players[i].playerHand.setTurned(true);
        }
    }
}

void Poker::PokerGame::initGameState(RenderWindow& window) {
    info.betPool = 0;
    info.callAmount = 5;
    info.interactionTime =  milliseconds(200);
    info.phase = 0;
    info.turn = 0;
    info.winnerIndex = -1;
}

void Poker::PokerGame::initUI(RenderWindow& window) {
    display.font.loadFromFile(Game::FontPaths::blackLivesFont);
    display.foldPressed = false;
    
    display.inputRect.setOutlineColor(Color::Blue);
	display.inputRect.setOutlineThickness(10.f * windowScale.y);
	display.inputRect.setSize(Vector2f(200.f, 50.f) * windowScale);
	display.inputRect.setPosition({50 * windowScale.x,
     window.getSize().y - display.inputRect.getSize().y - 50 * windowScale.y});

    display.inputText.setFont(display.font);
	display.inputText.setCharacterSize(40.f * windowScale.x);
	display.inputText.setFillColor(Color::Black);
	display.inputText.setPosition(display.inputRect.getPosition());

    display.callBox.setSize(Vector2f(100.f, 50.f) * windowScale);
	display.callBox.setOutlineColor(Color::Blue);
	display.callBox.setOutlineThickness(5.f * windowScale.y);
	display.callBox.setPosition(
		window.getSize().x - display.callBox.getSize().x - 50.f * windowScale.x,
		window.getSize().y - display.callBox.getSize().y - 50.f * windowScale.y
	);
	display.callText.setFont(display.font);
	display.callText.setCharacterSize(30.f * windowScale.x);
	display.callText.setPosition(display.callBox.getPosition());
	display.callText.setFillColor(Color::Blue);
	display.callText.setString(display.callString);

    display.foldBox.setSize(display.callBox.getSize());
	display.foldBox.setOutlineColor(Color::Red);
	display.foldBox.setOutlineThickness(5.f * windowScale.y);
	display.foldBox.setPosition(display.callBox.getPosition().x,
     display.callBox.getPosition().y - display.callBox.getSize().y - 30.f * windowScale.y);
    
    display.foldText.setFont(display.font);
	display.foldText.setCharacterSize(30.f * windowScale.x);
	display.foldText.setFillColor(Color::Red);
	display.foldText.setPosition(display.foldBox.getPosition());
	display.foldText.setString("FOLD");

    display.t_callAmount.setFont(display.font);
	display.t_callAmount.setCharacterSize(20.f * windowScale.x);
	display.t_callAmount.setFillColor(Color::White);
	display.t_callAmount.setPosition(deck.getPositon().x, deck.getPositon().y - 50.f * windowScale.y);
	display.t_callAmount.setString(std::to_string(info.callAmount));
}

void Poker::PokerGame::betPhase(SOCKET* acceptSock) {
	packet1 pack;
	if (players[info.turn].playerHand.getFolded()) {
		info.turn++;
	}
	else if (players[info.turn].playerHand.getIsPlayer()) {
		if ((Keyboard::isKeyPressed(Keyboard::Enter)) && info.interactionClock.getElapsedTime() > info.interactionTime) {
			info.interactionClock.restart();
			if (display.foldPressed) {
				players[info.turn].playerHand.setFolded(true);
			}
			else if (players[info.turn].isRaising && display.input != "") {
				int raiseAmount = std::stoi(display.input);
				pack.raiseAmount = raiseAmount;

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
				pack.raiseAmount = 0;
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

			pack.isRaising = players[info.turn].isRaising;
			send(*acceptSock, (char*)&pack, sizeof(packet1), 0);

			info.turn++;
		}
	}
	else if(players[info.turn].isPlayer){
		//IN PROGRESS
		if(threadProgress == 0){
			recieve = std::thread(&Poker::PokerGame::recvThread, this, acceptSock, &threadProgress);
		}
		else if(threadProgress > 1){
			recieve.join();
			threadProgress = 0;
			info.turn++;
		}
	}
	else {
		int randNum = std::rand() % 10 + 1;
		players[info.turn].isRaising = (randNum > 8);
		if ((players[info.turn].betMoney - info.callAmount) < 0) {
			players[info.turn].isRaising = false;
		}
		if (players[info.turn].isRaising) {
			int raiseAmount = rand() % (players[info.turn].betMoney - info.callAmount);
			int diff = raiseAmount + (info.callAmount - players[info.turn].betAmount);
			players[info.turn].betMoney -= diff;
			info.betPool += diff;
			info.callAmount += raiseAmount;
			players[info.turn].betAmount += diff;
		}
		else {
			if (players[info.turn].betAmount < info.callAmount) {
				if (players[info.turn].betMoney < (info.callAmount - players[info.turn].betAmount)) {
					players[info.turn].betAmount += players[info.turn].betMoney;
					info.betPool += players[info.turn].betMoney;
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
		players[info.turn].playerHand.hasChosen = false;
		players[info.turn].t_betMoney.setString(std::to_string(players[info.turn].betMoney));
		info.turn++;
	}
}

void Poker::PokerGame::discardPhase(SOCKET* acceptSock){
	auto& hand = players[info.turn].playerHand; 

	if (hand.getFolded()){
		info.turn++;
	}
	else if (players[info.turn].playerHand.getIsPlayer()) {
		if (Keyboard::isKeyPressed(Keyboard::Enter) && info.interactionClock.getElapsedTime() > info.interactionTime) {
			info.interactionClock.restart();
			hand.discardCards();
			hand.setHandType();
			players[info.turn].t_handType.setString(Poker::Hand::typesMap.at(hand.getHandType()));
			info.turn++;
		}
	}
	else {
		hand.discardCards();
		hand.setHandType();
		hand.setTurned(true);
		players[info.turn].t_handType.setString(Poker::Hand::typesMap.at(hand.getHandType()));
		hand.hasChosen = false;
		info.turn++;
	}
}

void Poker::PokerGame::endPhase(SOCKET* acceptSock) {
	Poker::Hand *winner = &players[0].playerHand;
	if (!info.end) {
		info.winnerIndex = 0;

		for (size_t i = 0; i < 4; i++) {
			auto& hand = players[i].playerHand;

			if (hand.getFolded()) {
				if (info.winnerIndex == i) {
					info.winnerIndex++;
					winner = &players[i + 1].playerHand;
				}
				continue;
			}
			if (winner->getHandType() < hand.getHandType()) {
				winner = &hand;
				info.winnerIndex = i;
			}
			else if (winner->getHandType() == hand.getHandType()) {
				if (winner->getHandType() == Poker::Hand::Flush) {
					if (hand.getHighSuit() > winner->getHighSuit()) {
						winner = &hand;
						info.winnerIndex = i;
					}
				}
				else if (hand.getHighCard() > winner->getHighCard()) {
					winner = &hand;
					info.winnerIndex = i;
				}
			}
		}
		for (size_t i = 0; i < 4; i++) {
			auto& hand = players[i].playerHand;

			if (i == info.winnerIndex) {
				players[i].t_handType.setString("Winner: " + static_cast<std::string>(players[i].t_handType.getString()));
				players[i].t_handType.setFillColor(Color::Green);
			}
			else {
				players[i].t_handType.setFillColor(Color::Red);
			}
			hand.setFolded(false);
			hand.setTurned(false);
			players[i].betAmount = 0;
		}
		info.end = true;
		display.foldPressed = false;
	}
	if (Keyboard::isKeyPressed(Keyboard::Enter) && info.interactionClock.getElapsedTime() > info.interactionTime) {
		info.interactionClock.restart();
		deck.reset();

		info.phase++;
		players[info.winnerIndex].betMoney += info.betPool;
		info.winnerIndex = 0;
		info.betPool = 0;
		info.callAmount = 5;
		info.end = false;
		for (size_t i = 0; i < 4; i++) {
			auto& hand = players[i].playerHand;
			for (int j = 0; j < 5; j++) {
				players[i].playerHand[j].getSprite().setColor(Color::White);
			}
			hand.setDeck(&deck);
			hand.setHandType();
			players[i].t_handType.setFillColor(Color::Blue);
			players[i].t_handType.setString(Poker::Hand::typesMap.at(hand.getHandType()));
			players[i].t_betMoney.setString(std::to_string(players[i].betMoney));
			if (!hand.getIsPlayer()) {
				hand.setTurned(true);
			}
			if (players[i].betMoney < 1) {
				players[i].bust = true;
			}
		}
	}
}

void Poker::PokerGame::phaseChange() {
	if (info.turn > 3) {
		info.turn = 0;
		info.phase++;
		bool allCall;
		for (size_t i = 0; i < 4; i++) {
			if (players[i].playerHand.getFolded()) {
				continue;
			}
			if (players[i].betAmount < info.callAmount && players[i].betMoney > 0) {
				allCall = false;
				break;
			}
			allCall = true;
		}
		if (!allCall) {
			info.phase--;
		}
	}
	if (info.phase > 3) {
		info.phase = 0;
		info.turn = 0;
	}
}

void Poker::PokerGame::displayInteraction(Event& anEvent) {
	auto& hand = players[info.turn].playerHand;

	if (true) {
		if ((info.phase == 0 || info.phase == 2)) {

			if (Mouse::isButtonPressed(Mouse::Left) &&
				info.interactionClock.getElapsedTime() > info.interactionTime) {
				info.interactionClock.restart();

				if (mouseCircle.getGlobalBounds().intersects(display.callBox.getGlobalBounds())) {
					if (display.callText.getString() == display.callString) {
						players[info.turn].isRaising = true;
						display.callText.setString(display.raiseString);
					}
					else {
						players[info.turn].isRaising = false;
						display.callText.setString(display.callString);
					}
				}
				else if (mouseCircle.getGlobalBounds().intersects(display.foldBox.getGlobalBounds()) &&
						 !players[info.turn].isRaising) {
					
					display.foldPressed = true;
				}
				if (mouseCircle.getGlobalBounds().intersects(display.inputRect.getGlobalBounds())) {
					display.isWriting = true;
				}
				else {
					display.isWriting = false;
				}
			}
			if (display.isWriting && players[info.turn].isRaising) {
				std::string validNums = "1234567890";

				if (anEvent.type == Event::TextEntered && 
					info.interactionClock.getElapsedTime() > info.interactionTime) {
					info.interactionClock.restart();
					if (anEvent.text.unicode == '\b') {
						if (display.inputText.getString() != "") {
							display.input.erase(display.input.size() - 1);
						}
					}
					else {
						display.input += anEvent.text.unicode;

						if (display.input.find_first_of(validNums) == std::string::npos) {
							display.input.erase(display.input.size() - 1);
						}
					}
				}
				display.inputText.setString(display.input);
			}
			else {
				display.input = "";
				display.inputText.setString(display.input);
			}
		}
	}
}

void Poker::PokerGame::draw(RenderWindow& window) {
	window.clear();
	for (size_t i = 0; i < 4; i++) {
		auto& hand = players[i].playerHand;
		hand.drawTo(window);
		window.draw(players[i].t_betMoney);
		if (hand.getIsPlayer() || info.phase == 3) {
			window.draw(players[i].t_handType);
		}
	}
	deck.drawTo(window);
	if (info.phase == 0 || info.phase == 2) {
		window.draw(display.callBox);
		window.draw(display.callText);
		if (players[info.turn].isRaising) {
			window.draw(display.inputRect);
			window.draw(display.inputText);
		}
		else {
			window.draw(display.foldBox);
			window.draw(display.foldText);
		}
	}
	window.draw(display.t_callAmount);
	window.display();
}

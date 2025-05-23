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
	int total = 0;
	for(int i = 0; i < 4; i++){
		if(players[i].isPlayer){
			total += 1;
		}
	}
	std::cout << total;
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

			players[info.turn].playerHand.setFolded(true);
		}

		mouseCircle.setPosition(Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y));
		players[info.turn].playerHand.updateMouse(mouseCircle);
		turnPointer.setPosition({players[info.turn].playerHand.getPosition().x,
			 players[info.turn].playerHand.getPosition().y + players[info.turn].playerHand.getSize().y * windowScale.x});
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

		for(int j = 0; j < 5; j++){
			players[i].playerHand.pat(j) = &deck.at(std::format("{}{}",
			 							Suits::suit.at(initPack.cards[i][j].second), initPack.cards[i][j].first));

		}
		players[i].playerHand.sortCards();

    }
	players[you].playerHand.setIsPlayer(true);
	for(int i = 0; i < initPack.playerNum; i++){
		players[i].isPlayer = true;
	}
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
    info.interactionTime =  milliseconds(150);
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
// NOTE: Refactor this later to have the calculations for the non-players to be done on server side. - DONE
void Poker::PokerGame::betPhase(SOCKET* acceptSock) {
	packet1 pack;
	if (players[info.turn].playerHand.getFolded()) {
		info.turn++;
	}
	else if (players[info.turn].playerHand.getIsPlayer()) {
		if ((Keyboard::isKeyPressed(Keyboard::Enter)) && info.interactionClock.getElapsedTime() > info.interactionTime) {
			info.interactionClock.restart();
			if (display.foldPressed) {
				pack.folded = true;
				players[info.turn].playerHand.setFolded(true);
			}
			else if (players[info.turn].isRaising && display.input != "") {
				int raiseAmount = std::stoi(display.input);
				
				int diff = raiseAmount + (info.callAmount - players[info.turn].betAmount);
				if (diff > players[info.turn].betMoney) {
					diff = players[info.turn].betMoney;
					raiseAmount = diff - info.callAmount;
				}
				info.callAmount += raiseAmount;
				players[info.turn].betMoney -= diff;
				info.betPool += diff;

				players[info.turn].betAmount += diff;
				pack.raiseAmount = raiseAmount;

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
			send(*acceptSock, (char*)&info.phase, sizeof(int), 0);

			send(*acceptSock, (char*)&pack, sizeof(packet1), 0);
			recv(*acceptSock, (char*)&pack, sizeof(packet1), 0);


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
	else if(you == 0){
		int randNum = std::rand() % 10 + 1;
		players[info.turn].isRaising = (randNum > 8);
		if ((players[info.turn].betMoney - info.callAmount) <= 0) {
			players[info.turn].isRaising = false;
		}
		if (players[info.turn].isRaising) {
			int raiseAmount = rand() % (players[info.turn].betMoney - info.callAmount);
			pack.raiseAmount = raiseAmount;
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
		pack.isRaising = players[info.turn].isRaising;
		int t = 0;
		send(*acceptSock, (char*)&info.phase, sizeof(int), 0);

		send(*acceptSock, (char*)&pack, sizeof(packet1), 0);
		recv(*acceptSock, (char *)&pack, sizeof(packet1), 0);
		info.turn++;
	}
	else{
		int recvCount;
		if ((recvCount = recv(*acceptSock, (char *)&pack, sizeof(packet1), 0)) != SOCKET_ERROR) {
			players[info.turn].isRaising = pack.isRaising;
			
			if (players[info.turn].isRaising) {
				int raiseAmount = pack.raiseAmount;
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
}

void Poker::PokerGame::discardPhase(SOCKET* acceptSock){
	auto& hand = players[info.turn].playerHand; 
	packet2 pack;
	if (hand.getFolded()){
		info.turn++;
	}
	else if (players[info.turn].playerHand.getIsPlayer()) {
		if (Keyboard::isKeyPressed(Keyboard::Enter) && info.interactionClock.getElapsedTime() > info.interactionTime) {
			std::vector<int> discarded = hand.getDiscarded();
			pack.discardNum = discarded.size();
			for(int i = 0; i < pack.discardNum; i++){
				pack.discarded[i] = discarded[i];
			}
			pack.index = you;
			send(*acceptSock, (char*)&info.phase, sizeof(int), 0);

			send(*acceptSock, (char*)&pack, sizeof(packet2), 0);
			if (threadProgress == 0) {
				recieve = std::thread(&Poker::PokerGame::recvThread, this, acceptSock, &threadProgress);
			}
			recieve.join();

			info.turn++;
			threadProgress = 0;
			hand.setHandType();
			players[info.turn].t_handType.setString(Hand::typesMap.at(hand.getHandType()));
			players[info.turn].playerHand.setTurned(false);
			info.interactionClock.restart();

		}
	}
	else if(players[info.turn].isPlayer){
		if(threadProgress == 0){
			recieve = std::thread(&Poker::PokerGame::recvThread,this , acceptSock, &threadProgress);
		}
		else if(threadProgress > 1){
			recieve.join();
			threadProgress = 0;
			info.turn++;

			hand.setHandType();
			players[info.turn].t_handType.setString(Hand::typesMap.at(hand.getHandType()));

		}
	}
	else if(you == 0){
		std::vector<int> discarded = hand.getDiscarded();
		pack.discardNum = discarded.size();
		hand.hasChosen = false;

		for (int i = 0; i < pack.discardNum; i++) {
			pack.discarded[i] = discarded[i];
		}
		pack.index = info.turn;
		
		send(*acceptSock, (char*)&info.phase, sizeof(int), 0);

		send(*acceptSock, (char *)&pack, sizeof(packet2), 0);
		if(threadProgress == 0){
			recieve = std::thread(&Poker::PokerGame::recvThread,this , acceptSock, &threadProgress);
		}
		recieve.join();
		threadProgress = 0;
		info.turn++;

		hand.setHandType();
		players[info.turn].t_handType.setString(Hand::typesMap.at(hand.getHandType()));

	}
	else{
		hand.hasChosen = false;
		if (threadProgress == 0) {
			recieve = std::thread(&Poker::PokerGame::recvThread, this, acceptSock, &threadProgress);
		}
		else if (threadProgress == 2) {
			recieve.join();
			threadProgress = 0;
			info.turn++;
			hand.setHandType();
			players[info.turn].t_handType.setString(Hand::typesMap.at(hand.getHandType()));
			std::cout << "Discard phase\n";

		}
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
			hand.setHandType();
			players[i].t_handType.setString(Hand::typesMap.at(hand.getHandType()));
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
	if (Keyboard::isKeyPressed(Keyboard::Enter) && info.interactionClock.getElapsedTime() > info.interactionTime && you == 0) {
		info.interactionClock.restart();
		packet3 pack;

		send(*acceptSock, (char *)&info.phase, sizeof(int), 0);
		recv(*acceptSock, (char *)&pack, sizeof(packet3), 0);

		players[info.winnerIndex].betMoney += info.betPool;
		deck.reset();
		info.winnerIndex = 0;
		info.betPool = 0;
		info.callAmount = 5;
		for (size_t i = 0; i < 4; i++) {
			for (int j = 0; j < 5; j++) {
				players[i].playerHand[j].getSprite().setColor(Color::White);
			}
		}
		for (int i = 0; i < 4; i++) {
			auto &hand = players[i].playerHand;
			for(int j = 0; j < 5; j++){
				hand.pat(j) = &deck.at(std::format("{}{}",
			 							Suits::suit.at(pack.cards[i][j].second), pack.cards[i][j].first));
			}
			players[i].t_handType.setFillColor(Color::Blue);
			players[i].t_handType.setString(Poker::Hand::typesMap.at(hand.getHandType()));
			players[i].t_betMoney.setString(std::to_string(players[i].betMoney));
			if (!hand.getIsPlayer()) {
				hand.setTurned(true);
			}
			else{
				hand.setTurned(false);
			}
			if (players[i].betMoney < 1) {
				players[i].bust = true;
			}

			hand.sortCards();
		}

		info.phase++;
		threadProgress = 0;
		info.end = false;

	}
	else if(you != 0){
		packet3 pack;

		if (threadProgress == 0) {
			recieve = std::thread(&Poker::PokerGame::recvThread, this, acceptSock, &threadProgress);
		}
		else if (threadProgress == 2) {
			recieve.join();
			threadProgress = 0;

			info.phase++;

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
				
				display.foldPressed = false;
				display.foldBox.setFillColor(Color::White);
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
					display.foldBox.setFillColor(Color(227, 51, 11));
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

						if (display.input.find_first_not_of(validNums) != std::string::npos) {
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
		if(info.phase != 3 && !hand.getIsPlayer()){
			hand.setTurned(true);
		}
		hand.drawTo(window);
		window.draw(players[i].t_betMoney);
		if (info.phase == 3) {
			window.draw(players[i].t_handType);
		}
	}
	window.draw(turnPointer);
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

int Poker::PokerGame::recvThread(SOCKET *acceptSock, int *threadActive){
	this->threadProgress = 1;
	auto bet = [&](){
		packet1 pack;
		int addrSize = sizeof(serverInfo);
		int bytes;
		if ((bytes = recv(*acceptSock, (char *)&pack, sizeof(packet1), 0)) != SOCKET_ERROR && bytes == sizeof(packet1)) {
			if(pack.folded){
				players[info.turn].playerHand.setFolded(true);
			}
			else{
				int raiseAmount = pack.raiseAmount;
				bool isRaising = pack.isRaising;
				std::cout << std::format("Bet bytes: {}\n", bytes);
				if (pack.isRaising){

					int diff = raiseAmount + (info.callAmount - players[info.turn].betAmount);
					if (diff > players[info.turn].betMoney) {
						diff = players[info.turn].betMoney;
						raiseAmount = diff;
					}
					players[info.turn].betMoney -= diff;
					info.betPool += diff;
					info.callAmount = diff > players[info.turn].betMoney ? (info.callAmount + players[info.turn].betMoney) :(info.callAmount + raiseAmount);
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
			players[info.turn].t_betMoney.setString(std::to_string(players[info.turn].betMoney));
		}
		else{
			std::cout << "Error in betphase\n";
			std::cout << WSAGetLastError();
		}

	};
	switch (info.phase) {
	case 0: 
		bet();
		break;
	case 1:{
		packet2 pack2;
		int recvCount;
		if((recvCount = recv(*acceptSock, (char *)&pack2, sizeof(packet2), 0) == sizeof(packet2)) != SOCKET_ERROR){
			for (int i = 0; i < 5; i++) {
				players[pack2.index].playerHand.pat(i) = &deck.at(std::format("{}{}",
														Suits::suit.at(pack2.cards[i].second), pack2.cards[i].first));
			}
			players[pack2.index].playerHand.sortCards();
		}
		for(size_t i = 0; i < 4; i++){
			if (!players[i].playerHand.getIsPlayer()){
				players[i].playerHand.setTurned(true);
			}
		}
	}
	break;

	case 2:
		bet();
		break;
	case 3: {
		packet3 pack;
		deck.reset();
		int bytes = 0;


		players[info.winnerIndex].betMoney += info.betPool;
		info.winnerIndex = 0;
		info.betPool = 0;
		info.callAmount = 5;
		for (size_t i = 0; i < 4; i++)
		{
			auto &hand = players[i].playerHand;
			for (int j = 0; j < 5; j++)
			{
				players[i].playerHand[j].getSprite().setColor(Color::White);
			}
		}
		if ((bytes = recv(*acceptSock, (char *)&pack, sizeof(packet3), 0)) != SOCKET_ERROR && bytes == sizeof(packet3)) {
			std::cout << std::format("end bytes: {}\n", bytes);

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
													   Suits::suit.at(pack.cards[i][j].second), pack.cards[i][j].first));
				}
				for(size_t i = 0; i < 4; i++){
					if (!players[i].playerHand.getIsPlayer()){
						players[i].playerHand.setTurned(true);
					}
				}
				hand.sortCards();

			}
			info.end = false;

			
		}
		else {
			throw FileError("Failed recieving data\n");
		}

		break;
	}

	default:
		break;
	}
	this->threadProgress = 2;
	return 0;
}

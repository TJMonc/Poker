#include "Game.h"

void Poker::PokerGame::init() {
    initDeck();
    initGameState();
    initUI();
    initPlayers();

}

void Poker::PokerGame::initDeck()
{
    Vector2f deckPos = {
        window.getSize().x - deck[0].getGlobalBounds().getSize().x,
        window.getSize().y - deck[0].getGlobalBounds().getSize().y
    };
    deckPos = (1.f / 2.f) * deckPos;
    deck.setPosition(deckPos);
}

void Poker::PokerGame::initPlayers() {
    for (int i = 0; i < 4; i++) {
        players[i].playerHand.setDeck(&deck);
        players[i].playerHand.setWindow(&window);
        players[i].isRaising = false;
        players[i].betMoney = 5000;
        players[i].betAmount = 0;
        players[i].bust = false;
        if (!players[i].playerHand.getIsPlayer()) {
            players[i].playerHand.setTurned(true);
        }
    }

    for(int i = 0; i < 4; i++){
        players[i].t_betMoney.setFont(display.font);
        players[i].t_betMoney.setCharacterSize(20.f * windowScale.x);
        players[i].t_betMoney.setPosition(players[i].playerHand.getPosition().x, players[i].playerHand.getPosition().y - 20 * windowScale.y);

        players[i].t_betMoney.setFillColor(Color::Cyan);
        players[i].t_betMoney.setString(std::to_string(players[i].betMoney));
    }

    for(int i = 0; i < 4; i++){
        Vector2f t_hTPos = {
            players[i].playerHand.getPosition().x,
            players[i].playerHand.getPosition().y + players[i].playerHand.getSize().y
        
        };
        players[i].t_handType.setFont(display.font);
		players[i].t_handType.setCharacterSize(20.f * windowScale.x);
		players[i].t_handType.setPosition(t_hTPos);
		players[i].t_handType.setFillColor(Color::Blue);
		players[i].t_handType.setString(Poker::Hand::typesMap.at(players[i].playerHand.getHandType()));
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
    players[0].playerHand.setIsPlayer(true);
}

void Poker::PokerGame::initGameState() {
    info.betPool = 0;
    info.callAmount = 5;
    info.interactionTime =  milliseconds(200);
    info.phase = 0;
    info.turn = 0;
    info.winnerIndex = -1;
}

void Poker::PokerGame::initUI() {
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

void Poker::PokerGame::betPhase() {
    	if(players[info.turn].playerHand.getFolded()){
			info.turn++;
		}
		else if (players[info.turn].playerHand.getIsPlayer()) {
			if ((Keyboard::isKeyPressed(Keyboard::Enter)) && info.interactionClock.getElapsedTime() > info.interactionTime) {
				info.interactionClock.restart();
				if(display.foldPressed){
					players[info.turn].playerHand.setFolded(true);
				}
				else if (players[info.turn].isRaising && display.input != "") {
					int raiseAmount = std::stoi(display.input);
					int diff = raiseAmount + (info.callAmount - players[info.turn].betAmount);
					if(diff > players[info.turn].betMoney){
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

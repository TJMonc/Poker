#include "Game.h"

void Poker::PokerGame::initDeck() {
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
    
}

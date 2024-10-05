#include "Game.h"

void Poker::PokerGame::init() {
    windowScale = Vector2f(window->getSize()) / Vector2f RES_768;
	mouseCircle.setRadius(5.f);
    mouseCircle.setOrigin(mouseCircle.getRadius(), mouseCircle.getRadius());
	mouseCircle.setPosition(Vector2f(Mouse::getPosition(*window)));
}

void Poker::PokerGame::initPlayers() {
    Vector2f mid = {
        window->getSize().x / 2 - players[0].hand.getSize().x / 2,
        window->getSize().y / 2 - players[0].hand.getSize().y / 2
        
    };

    Vector2f neg = {
        window->getSize().x - players[0].hand.getSize().x,
        window->getSize().y - players[0].hand.getSize().y
    };
    
    for (size_t i = 0; i < players.size(); i++) {
        players[i].hand.setDeck(deck.get());
        players[i].hand.setWindow(window.get());
        players[i].money = 5000;
    }
    players[0].hand.setPosition({mid.x, neg.y});
    players[1].hand.setPosition({0.f, mid.y});
    players[2].hand.setPosition({mid.x, 0.f});
    players[3].hand.setPosition({neg.x, mid.y});
}

void Poker::PokerGame::initUI() {
    userInterface->font.loadFromFile(Game::FontPaths::blackLivesFont);
    userInterface->inputText.setFont(userInterface->font);
    userInterface->inputText.setCharacterSize(40 * windowScale.x);
    userInterface->textBox.setSize({500.f * windowScale.x, 50.f * windowScale.y});
    userInterface->inputText.setFillColor(Color::Black);
    userInterface->textBox.setOutlineColor(Color::Blue);
    userInterface->textBox.setOutlineThickness(10.f);

    userInterface->handTypeText.setString(Hand::typesMap.at(players[0].hand.getHandType()));
    userInterface->handTypeText.setFillColor(userInterface->inputText.getFillColor());
    userInterface->handTypeText.setCharacterSize(30.f * windowScale.y);

    userInterface->betTypeBox.setOutlineColor(userInterface->textBox.getOutlineColor());
    userInterface->betTypeBox.setOutlineThickness(userInterface->inputText.getOutlineThickness());
    userInterface->betTypeBox.setSize(100.f * windowScale);
    userInterface->callOrRaise.setString("Call");

    userInterface->betTypeBox.setPosition(
        window->getSize().x - userInterface->betTypeBox.getSize().x * 5,
        window->getSize().y - userInterface->betTypeBox.getSize().y * 2);

    userInterface->callOrRaise.setPosition(
        userInterface->betTypeBox.getPosition().x + 20,
        userInterface->betTypeBox.getPosition().y + 40);
}

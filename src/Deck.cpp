#include "Deck.h"

Poker::Deck::Deck() {
	this->deck = (Card*)new Card[52];
	this->mySize = 52;
	int counter = 0;
	for (size_t i = 10; i < Suits::Spades + 1; i += 10) {
		for (size_t j = 2; j < CardNumbers::ace + 1; j++) {
			deck[counter].changeCard(j, i);
			std::unique_ptr<std::vector<std::string>> vect = std::move(Poker::Card::getCardBackPaths());
			deck[counter].setIsTurned(false);
			int lol = rand() % vect->size();

			deck[counter].setBackPath((*vect)[lol]);
			counter++;
		}
	}
	Game::loadSprite(deckText, "CardBacks/CardDeck.png");
	deckSprite.setTexture(deckText);
}

Poker::Deck::~Deck(){
	delete[] this->deck;
}

void Poker::Deck::drawTo(RenderWindow& aWindow) {
	window->draw(this->deckSprite);
}

void Poker::Deck::setWindow(RenderWindow* aWindow) {
	this->window = aWindow;
	this->windowScale = (Vector2f(window->getSize()) / Vector2f RES_768);
	deckSprite.setScale(windowScale * 0.2f);
	deckSprite.setOrigin(deckSprite.getGlobalBounds().getSize() / 2.f);
	for (size_t i = 0; i < this->mySize; i++) {
		deck[i].getSprite().setScale(windowScale * 0.2f);
	}
	
}

void Poker::Deck::setPosition(const Vector2f aPos) {
	this->position = aPos;
	deckSprite.setPosition(position);

}

const Vector2f& Poker::Deck::getPositon() const {
	return this->position;
}

Poker::Card& Poker::Deck::at(const int index) {
	return this->deck[index];

}

const Poker::Card& Poker::Deck::at(const int index) const
{
	return this->deck[index];
}

Poker::Card& Poker::Deck::at(const std::string aID)
{
	for (size_t i = 0; i < 52; i++) {
		if (deck[i].getID() != aID) {
			continue;
		}
		else {
			return deck[i];
		}
	}
	throw FileError("Main app failure due to Poker::Deck::at");
	return deck[0];
}

const Poker::Card& Poker::Deck::at(const std::string aID) const
{
	for (size_t i = 0; i < 52; i++) {
		if (deck[i].getID() != aID) {
			continue;
		}
		else {
			return deck[i];
		}
	}
	throw FileError("Main app failure due to Poker::Deck::at");
	return deck[0];
}

Poker::Card& Poker::Deck::operator[](const int index) {
	return this->deck[index];
}
const Poker::Card& Poker::Deck::operator[](const int index) const {
	return this->deck[index];
}

const Poker::Card& Poker::Deck::operator[](const std::string aID) const {
	for (size_t i = 0; i < 52; i++) {
		if (deck[i].getID() != aID) {
			continue;
		}
		else {
			return deck[i];
		}
	}
	throw std::exception();
}

Poker::Card& Poker::Deck::operator[](const std::string aID) {
	for (size_t i = 0; i < 52; i++) {
		if (deck[i].getID() != aID) {
			continue;
		}
		else {
			return deck[i];
		}
	}
	throw FileError("Main app failure due to Poker::Deck::operato[]");
	return deck[0];
}

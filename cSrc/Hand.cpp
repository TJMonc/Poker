#include "Hand.h"
#include <format>
const std::unordered_map<int, std::string> Poker::Hand::typesMap = {
	{HandTypes::High, "High Card"},
	{HandTypes::Pair, "Pair"},
	{HandTypes::TwoPair, "Two Pair"},
	{HandTypes::ThreeKind, "Three of a Kind"},
	{HandTypes::Flush, "Flush"},
	{HandTypes::Straight, "Straight"},
	{HandTypes::FullHouse, "Full House"},
	{HandTypes::FourKind, "Four of a Kind"},
	{HandTypes::StraightFlush, "Straight Flush"},

};

Poker::Hand::Hand(Deck* aDeck)
: isPlayer(false) {
	this->setDeck(aDeck);
}

//You should only pass the cards that are in the hand with replaceCard.
void Poker::Hand::replaceCard() {
	std::vector<Card*> pastNums;
	for (size_t i = 0; i < 5; i++) {
		int randNum = rand() % deck->getSize();
		this->pat(i)->getSprite().setColor(Color::White);
		while (deck->at(randNum).isInHand()) {
			randNum = rand() % deck->getSize();
		}
		
		this->pat(i)->setIsTurned(true);
		hand[i] = &deck->at(randNum);
		this->deck->at(randNum).setInHand(true);
	}
	for (size_t i = 0; i < pastNums.size(); i++) {
		pastNums.at(i)->setInHand(false);
	}
	discarded.clear();
	this->sortCards();
}

void Poker::Hand::setWindow(RenderWindow* aWindow) {
	this->window = aWindow;
	windowScale = { Vector2f(window->getSize()) / Vector2f RES_768 };
	relativePos = 35.f * windowScale.x;
	this->setPosition({ 0,0 });
	Vector2f handSize;
	float xFactor = windowScale.x * 10.f;
	size = { 0, hand[0]->getGlobalBounds().getSize().y };
	for (size_t i = 0; i < 5; i++) {
		Vector2f trueChoice = Vector2f(hand[i]->getGlobalBounds().getSize().x - xFactor, hand[i]->getGlobalBounds().getSize().y);
		Vector2f falseChoice = Vector2f(relativePos - xFactor, hand[i]->getGlobalBounds().getSize().y);

		handSize = (i == 5 - 1) ? trueChoice : falseChoice;
		this->visibleHand[i].setSize(handSize);
		this->size = { size.x + visibleHand[i].getSize().x, size.y};
	}
}

void Poker::Hand::drawTo(RenderWindow& aWindow) {
	for (int i = 0; i < 5; i++) {
		hand[i]->drawTo(aWindow);
	}
}

void Poker::Hand::updateMouse(CircleShape& mPointer) {
	if(isFolded){
		for(size_t i = 0; i < 5; i++){
			hand[i]->getSprite().setColor(Color::Red);
		}
	}
	else if (isPlayer) {
		for (size_t i = 0; i < 5; i++) {
			if (visibleHand[i].getGlobalBounds().intersects(mPointer.getGlobalBounds())) {
				hand[i]->getSprite().setColor(Color::Green);
				if (interactionClock.getElapsedTime() > interactionTime) {
					if (Mouse::isButtonPressed(Mouse::Left) && isDiscarded(i)) {
						unDiscard(i);
						hand[i]->getSprite().setColor(Color::White);
					}
					else if (Mouse::isButtonPressed(Mouse::Left)) {
						discarded.push_back(i);
					}
					interactionClock.restart();
				}
			}
			else {
				hand[i]->getSprite().setColor(Color::White);
			}
			if (isDiscarded(i)) {
				hand[i]->getSprite().setColor(Color::Blue);
			}
		}
	}
	else if(!hasChosen){
		npBehavior();
	}
}

Vector2f Poker::Hand::getSize() {
	return this->size;
}

void Poker::Hand::setDeck(Deck* aDeck) {
	this->deck = aDeck;
	for (size_t i = 0; i < 5; i++) {
		int randNum = rand() % 52;
		hand[i] = &deck->at(randNum);
		hand[i]->setInHand(true);

	}
	this->sortCards();

}

void Poker::Hand::setTurned(const bool isTurned) {
	if (isTurned) {
		for (size_t i = 0; i < 5; i++) {
			hand[i]->setIsTurned(true);
		}
	}
	else {
		for (size_t i = 0; i < 5; i++) {
			hand[i]->setIsTurned(false);
		}
	}
}

void Poker::Hand::setIsPlayer(const bool aIsPlayer) {
	this->isPlayer = aIsPlayer;
}

bool Poker::Hand::isDiscarded(int index) {
	for (size_t i = 0; i < discarded.size(); i++) {
		if (discarded[i] == index) {
			return true;
		}
	}
	return false;
}

void Poker::Hand::unDiscard(int index) {
	for (size_t i = 0; i < discarded.size(); i++) {
		if (discarded[i] == index) {
			discarded.erase(discarded.begin() + i);
		}
	}
}

void Poker::Hand::discardCards() {
	std::vector<Card*> pastNums;
	for (size_t i = 0; i < discarded.size(); i++) {
		int randNum = rand() % 52;
		int index = discarded[i];
		this->pat(index)->getSprite().setColor(Color::White);

		this->pat(index)->setIsTurned(false);
		hand[index] = &deck->at(randNum);
		this->deck->at(randNum).setInHand(true);

	}
	for (size_t i = 0; i < pastNums.size(); i++) {
		pastNums.at(i)->setInHand(false);

	}
	discarded.clear();
	this->sortCards();
}

std::vector<int> Poker::Hand::getDiscarded() {
    return this->discarded;
}

void Poker::Hand::setPosition(Vector2f aPos) {
	this->position = aPos;
	for (size_t i = 0; i < 5; i++) {
		hand[i]->setPosition(aPos);
		visibleHand[i].setPosition(hand[i]->getPosition());
		aPos = { aPos.x + relativePos, aPos.y };
	}
}

const Vector2f Poker::Hand::getPosition() const {
	return this->position;
}

const int Poker::Hand::getIDIndex(std::string aID) const {
	for (size_t i = 0; i < 5; i++) {
		if (hand[i]->getID() == aID) {
			return i;
		}
	}
	return -1;
}

const int Poker::Hand::getHandType() const {
	return this->handType;
}

const bool Poker::Hand::getIsPlayer() const {
    return this->isPlayer;
}

void Poker::Hand::sortCards() {
	for (size_t i = 0; i < 4; i++) {
		int min = i;
		for (size_t j = i; j < 5; j++) {
			if (hand[j]->getNumber() < hand[min]->getNumber()) {
				min = j;
			}
		}
		if (min != i) {
			Card* temp = hand[i];
			hand[i] = hand[min];
			hand[min] = temp;
		}
	}
	for (size_t i = 0; i < 4; i++) {
		int min = i;
		for (size_t j = i; j < 5; j++) {
			if (hand[j]->getNumber() == hand[min]->getNumber() && hand[j]->getSuite() < hand[min]->getSuite()) {
				min = j;
			}
		}
		if (min != i) {
			Card* temp = hand[i];
			hand[i] = hand[min];
			hand[min] = temp;
		}
	}
	this->highCard = CardNumbers::Number(2);
	this->setHighSuit();
	this->setHandType();
	this->setPosition(position);
}

std::string Poker::Hand::setHighSuit() {
	highSuit = Suits::Suit(hand[4]->getSuite());

	return hand[4]->getID();
}

int Poker::Hand::getHighCard() const {
	return highCard;
}

int Poker::Hand::getHighSuit() const {
	return highSuit;
}

bool Poker::Hand::checkFlush() {
	for (size_t i = 0; i < 5; i++) {
		if (hand[0]->getSuite() != hand[i]->getSuite()) {
			return false;
		}
	}
	return true;
}

bool Poker::Hand::checkStraight() {
	int startNum = hand[0]->getNumber();
	for (size_t i = 1; i < 5; i++) {
		startNum++;
		if (hand[i]->getNumber() != startNum) {
			return false;
		}
	}
	return true;
}

bool Poker::Hand::checkStraightFlush() {
	if (checkStraight() && checkFlush()) {
		return true;
	}
	else {
		return false;
	}
}

bool Poker::Hand::checkPair() {
	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 1 + i; j < 5; j++) {
			if (hand[i]->getNumber() == hand[j]->getNumber()) {
				highCard = (CardNumbers::Number)hand[i]->getNumber();
				return true;
			}
		}
	}
	highCard = (CardNumbers::Number)hand[4]->getNumber();
	return false;
}

bool Poker::Hand::checkTwoPair() {
	int pairCounter = 0;
	for (size_t i = 0; i < 4; i++) {
		for (size_t j = 1 + i; j < 5; j++) {
			if (hand[i]->getNumber() == hand[j]->getNumber()) {
				if (highCard < hand[i]->getNumber()) {
					highCard = (CardNumbers::Number)hand[i]->getNumber();
				}
				pairCounter++;
				break;
			}
		}
	}
	if (pairCounter == 2) {
		return true;
	}
	else {
		highCard = (CardNumbers::Number)hand[4]->getNumber();
		return false;
	}
}

bool Poker::Hand::checkKind(int number) {
	int counter = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = i; j < 5; j++) {
			if (hand[i]->getNumber() == hand[j]->getNumber()) {
				highCard = (CardNumbers::Number)hand[i]->getNumber();
				counter++;
				if (counter >= number){
					return true;
				}
			}
			else {
				counter = 0;
				break;
			}
		}
		counter = 0;
	}
	highCard = (CardNumbers::Number)hand[4]->getNumber();
	return false;
}

bool Poker::Hand::checkFullHouse() {
	if (checkKind(3)) {
		for (size_t i = 0; i < 4; i++) {
			for (size_t j = 1 + i; j < 5; j++) {
				if (hand[i]->getNumber() == hand[j]->getNumber() && hand[j]->getNumber() != highCard) {
					highCard = (CardNumbers::Number)hand[i]->getNumber();
					return true;
				}
			}
		}
		highCard = (CardNumbers::Number)hand[4]->getNumber();
		return false;
	}

	else {
		return false;
	}
}

void Poker::Hand::setHandType() {
	if (checkStraightFlush()) {
		this->handType = StraightFlush;
	}
	else if (checkStraight()) {
		this->handType = Straight;
	}
	else if (checkFlush()) {
		this->handType = Flush;
	}
	else if (checkFullHouse()) {
		this->handType = FullHouse;
	}
	else if (checkKind(4)) {
		this->handType = FourKind;
	}
	else if (checkKind(3)) {
		this->handType = ThreeKind;
	}
	else if (checkTwoPair()) {
		this->handType = TwoPair;
	}
	else if (checkPair()) {
		this->handType = Pair;
	}
	else {
		highCard = (CardNumbers::Number)hand[4]->getNumber();
		this->handType = High;
	}
}



Poker::Card*& Poker::Hand::pat(const int index) {
	return hand[index];
}

const Poker::Card& Poker::Hand::at(const int index) const {
	return *hand[index];
}

Poker::Card& Poker::Hand::at(const std::string aID) {
	for (size_t i = 0; i < 5; i++) {
		if (hand[i]->getID() == aID) {
			return *hand[i];
		}
	}
	throw FileError("Main app failure due to Poker::Hand::at");
	return *hand[0];
}

const Poker::Card& Poker::Hand::at(const std::string aID) const
{
	for (size_t i = 0; i < 5; i++) {
		if (hand[i]->getID() == aID) {
			return *hand[i];
		}
	}
	throw FileError("Main app failure due to Poker::Hand::at");
	return *hand[0];

}

Poker::Card& Poker::Hand::operator[](const int index) {
	return *hand[index];
}

const Poker::Card& Poker::Hand::operator[](const int index) const {
	return *hand[index];

}

Poker::Card& Poker::Hand::operator[](const std::string ID) {	
	for (size_t i = 0; i < 5; i++) {
		if (hand[i]->getID() == ID) {
			return *hand[i];
		}
	}
	throw FileError("Main app failure due to Poker::Hand::operator[]");
	return *hand[0];
}

const Poker::Card& Poker::Hand::operator[](const std::string ID) const {
	for (size_t i = 0; i < 5; i++) {
		if (hand[i]->getID() == ID) {
			return *hand[i];
		}
	}
	throw FileError("Main app failure due to Poker::Hand::operator[]");
	return *hand[0];
}

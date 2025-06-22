
#include "Cards.h"


const std::unordered_map<int, std::string> Poker::Suits::suit = { {Spades, "spades"}, {Hearts, "hearts" }, {Diamonds, "diamonds"}, {Clubs, "clubs"} };



Poker::Card::Card(const int aNumber, const int aSuit)
: number(CardNumbers::Number(aNumber)), suit(Suits::Suit(aSuit)), inHand(false), isTurned(false){
	cardText = Texture();
	this->changeCard(number, suit);
	texturePath = "";
	std::unique_ptr<std::vector<std::string>> vect = std::move(getCardBackPaths());
	int lol = rand() % vect->size();
	this->backPath = (*vect)[lol];
}

Poker::Card::Card()
	:Card(CardNumbers::ace, Suits::Spades) {

}

std::unique_ptr<std::vector<std::string>> Poker::Card::getCardBackPaths() {
	std::unique_ptr<std::ifstream> file = std::move(openReadFile("CardBackPaths.csv"));
	std::vector<std::string> cardBackPaths;
	std::string fileLine = "";

	std::getline(*file, fileLine);
	while (std::getline(*file, fileLine)) {
		std::stringstream ss(fileLine);
		std::string path;
		std::getline(ss, path, ',');
		std::getline(ss, path, ',');
		cardBackPaths.push_back(path);
	}
	file->close();
	return std::make_unique<std::vector<std::string>>(cardBackPaths);
}

std::string Poker::Card::getCardBackPath(const std::string color) {
	std::unique_ptr<std::ifstream> file = std::move(openReadFile("CardBackPaths.csv"));
	std::string fileLine;
	while (std::getline(*file, fileLine)) {
		if (fileLine.find(color) != std::string::npos) {
			std::stringstream ss(fileLine);
			std::string breaker;
			std::getline(ss, breaker, ',');
			std::getline(ss, breaker, ',');

			file->close();

			return breaker;
		}
	}
	throw FileError("Main app failure due to Poker::Card::getCardPath");
	return NULL;
}

std::string Poker::Card::getCardPath(const CardNumbers::Number aCardNum, const Suits::Suit aSuit) {
	std::unique_ptr<std::ifstream> file = std::move(openReadFile("CardPaths.csv"));
	std::string fileLine;
	while (std::getline(*file, fileLine)) {
		if (fileLine.find(Suits::suit.at(aSuit) + "," + std::to_string(aCardNum)) != std::string::npos) {
			std::stringstream s(fileLine);
			std::string breaker;
			std::getline(s, breaker, ',');
			std::getline(s, breaker, ',');
			std::getline(s, breaker, ',');
			
			return breaker;
		}
	}
	
	throw FileError("Main app failure due to Poker::Card::getCardPath");
	return "no";
}

void Poker::Card::drawTo(RenderWindow& window) {
	window.draw(cardSprite);
}

void Poker::Card::setWindow(RenderWindow* aWindow) {
	this->window = aWindow;
	windowScale = Vector2f(window->getSize()) / Vector2f(RES_768_X, RES_768_Y);
	this->cardSprite.setScale(windowScale * 0.2f);
}

Sprite& Poker::Card::getSprite() {
	return this->cardSprite;
}

FloatRect Poker::Card::getGlobalBounds() {
	return cardSprite.getGlobalBounds();
}

const bool& Poker::Card::isInHand() const {
	return this->inHand;
}
void Poker::Card::setInHand(bool aInHand) {
	this->inHand = aInHand;
}

const int Poker::Card::getNumber() const {
	return this->number;
}
const int Poker::Card::getSuite() const {
	return this->suit;
}


void Poker::Card::changeCard(const int aNumber, const int aSuit) {
	this->number = CardNumbers::Number(aNumber);
	this->suit = Suits::Suit(aSuit);

	this->texturePath = getCardPath(number, suit);
	Game::loadSprite(cardText, texturePath);
	this->ID = Suits::suit.at(aSuit) + std::to_string(aNumber);
	
	cardSprite.setTexture(cardText);

}

void Poker::Card::setIsTurned(bool aPath) {
	if (aPath) {
		Game::loadSprite(this->cardText, this->backPath);
		this->cardSprite.setTexture(cardText);
	}
	else {
		Game::loadSprite(this->cardText, this->texturePath);
		this->cardSprite.setTexture(cardText);
	}
}
void Poker::Card::setPosition(const Vector2f aPos) {
	this->position = aPos;
	cardSprite.setPosition(position);
}

void Poker::Card::setPosition(const float xPos, const float yPos) {
	this->position = { xPos, yPos };
	cardSprite.setPosition(position);

}

void Poker::Card::setBack(const std::string color) {
	this->backPath = this->getCardBackPath(color);

}

void Poker::Card::setBackPath(const std::string aPath) {
	this->backPath = aPath;
}

const Vector2f& Poker::Card::getPosition() const {
	return this->position;
}

std::string Poker::Card::getID() const {
	return this->ID;
}

std::ostream& operator<<(std::ostream& os, const Poker::Card& aCard) {
	os << aCard.getID();
	return os;
}
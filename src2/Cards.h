#pragma once
#include "Misc.h"
#include <WinSock2.h>

using namespace sf;
namespace Poker {
	struct CardNumbers {
		enum Number {
			two = 2,
			three,
			four,
			five,
			six,
			seven,
			eight,
			nine,
			ten,
			jack,
			queen,
			king,
			ace
		};
	};

	struct Suits {
		enum Suit{
			Clubs = 10,
			Diamonds = 20,
			Hearts = 30,
			Spades = 40,
		};
		const static std::unordered_map<int, std::string> suit; 

	};

	class Card {
	private:
		RenderWindow* window;
		Vector2f windowScale;
		Texture cardText;
		Sprite cardSprite;
		CardNumbers::Number number;
		Suits::Suit suit;
		bool inHand;
		bool isTurned;
		Vector2f position;
		std::string texturePath;
		std::string backPath;
		std::string ID;
	public:
		static std::unique_ptr<std::vector<std::string>> getCardBackPaths();
		static std::string getCardBackPath(const std::string color);
		static std::string getCardPath(const CardNumbers::Number aCardNum, const Suits::Suit aSuit);

	public:
		Card(const int aNumber, const int aSuit);
		Card();
		void drawTo(RenderWindow& window);
		void setWindow(RenderWindow* aWindow);

		Sprite& getSprite();
		FloatRect getGlobalBounds();

		const bool& isInHand() const;
		void setInHand(bool aInHand);

		const int getNumber() const;
		const int getSuite() const;

		void changeCard(const int aNumber, const int aSuit);
		void setIsTurned(bool isTurned);
		void setPosition(const Vector2f aPos);
		void setPosition(const float xPos, const float yPos);
		void setBack(const std::string color);
		void setBackPath(const std::string aPath);
		const Vector2f& getPosition() const;
		std::string getID() const;


	};
}

std::ostream& operator<<(std::ostream& os, const Poker::Card& aCard);
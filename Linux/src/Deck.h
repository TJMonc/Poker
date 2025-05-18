#ifndef DECK_H
#define DECK_H
#include "Cards.h"
namespace Poker {
	class Deck {
	private:
		Card* deck;
		Card* objDeck[52];
		size_t mySize;
		Sprite deckSprite;
		Texture deckText;
		Vector2f position;
		Vector2f windowScale;
		RenderWindow* window;
	public:
		Deck();
		~Deck();
		void drawTo(RenderWindow& aWindow);
		void remove(int index);
		void reset();
		void setWindow(RenderWindow* aWindow);
		void setPosition(const Vector2f aPos);
		const Vector2f& getPositon() const;
		int getSize() const { return mySize; }
		Card& at(const int index);
		const Card& at(const int index) const;
		Card& at(const std::string aID);
		const Card& at(const std::string aId) const;
		Card& operator[](const int index);
		const Card& operator[](const int index) const;
		const Card& operator[](const std::string aID) const;	//ID format is Suit-Number
		Card& operator[](const std::string aID);

	};
}

#endif


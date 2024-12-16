
#ifndef HAND_H
#define HAND_H
#include "Deck.h"

//TODO: Implement checks for hand type, highCard, and High suit;

namespace Poker {
	class Hand {
	public:
		enum HandTypes {
			High = 0,
			Pair,
			TwoPair,
			ThreeKind,
			Straight,
			Flush,
			FullHouse,
			FourKind,
			StraightFlush,
		};
		static const std::unordered_map<int, std::string> typesMap;
	private:
		RenderWindow* window;
		Vector2f windowScale;
		float relativePos;
		Vector2f position;
		Vector2f size;
		RectangleShape visibleHand[5];
		std::vector<int> discarded;
		
		Deck* deck;
		Card* hand[5];
		bool isPlayer = false;
		CardNumbers::Number highCard;
		CardNumbers::Number highCard2;
		Suits::Suit highSuit;
		HandTypes handType;
		Clock interactionClock;
		Time interactionTime = milliseconds(150);
		bool isFolded = false;
	public:
		bool hasChosen = false;
	private:
		void npBehavior() {
			if(isPlayer || hasChosen){
				return;
			}
			int discardNum = rand() % 5;
			if (discardNum != 5) {
				for (int i = 0; i < discardNum; i++) {
					discarded.push_back(i);
				}
			}
			else {
				for (int i = 0; i < discardNum; i++) {
					discarded.push_back(i);
				}
			}
		
			hasChosen = true;
		}

	public:
		Hand(Deck* deck);
		Hand() = default;
		
		void replaceCard();	//Uses the ID of a card taken in as argument to replace one card with a random card from deck		// A bit inefficient now that I think about it
		void setWindow(RenderWindow* aWindow);
		void drawTo(RenderWindow& aWindow);
		void updateMouse(CircleShape& mpointer);
		Vector2f getSize();

		void setDeck(Deck* deck);
		void setTurned(const bool isTurned);
		void setIsPlayer(const bool aIsPlayer);
		void setFolded(const bool &folded) {this->isFolded = folded;};
		bool isDiscarded(int index);	// Checks if a certain card's index is in the discarded vector
		void unDiscard(int index);		// Removes an index from the discarded vector

		void discardCards();			// Discards the index in the discarded hand
		void setPosition(Vector2f aPos);
		const Vector2f getPosition() const;
		const int getIDIndex(std::string aID) const;
		const int getHandType() const;
		const bool getIsPlayer() const;
		const bool getFolded() const {return this->isFolded;};
		void sortCards();
		
		std::string setHighSuit();
		int getHighCard() const;
		int getHighSuit() const;
		bool checkFlush();
		bool checkStraight();
		bool checkStraightFlush();
		bool checkPair();
		bool checkTwoPair();
		bool checkKind(int number);
		bool checkFullHouse();
		void setHandType();


		Card*& pat(const int index);
		const Card& at(const int index) const;
		Card& at(const std::string aID);
		const Card& at(const std::string aID) const;

		Card& operator[](const int index);
		const Card& operator[](const int index) const;
		Card& operator[](const std::string ID);
		const Card& operator[](const std::string ID) const;
		friend std::ostream& operator<<(std::ostream& out, const Hand& hand) {
			out
				<< "High Card: " << hand.highCard << "\n"
				<< "High Suit: " << Suits::suit.at(hand.highSuit) << "\n"
				<< "Hand Type: " << Hand::typesMap.at(hand.handType) << "\n";
			out << std::endl;
			
			return out;
		}
	};
}
#endif
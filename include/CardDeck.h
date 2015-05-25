#ifndef CARDDECK_H
#define CARDDECK_H

#include <vector>
#include <string>

enum Suit{ HEART, CLUB, SPADE, DIAMOND };
enum Type{ TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE };

struct Card {
  Suit suit; //the suit of the card
  Type type; //The type of the card
};

static const char* type_translated_to_text[] = {"two","three","four","five","six","seven","eight","nine","ten","jack","queen","king","ace"};
static const char* suit_translated_to_text[] = {"heart","club","spade","diamond"};

std::string cardToString( const Card &c );



class CardDeck {
 public:
  CardDeck();
  CardDeck( int seed );
  Card getNextCard();
  bool isEmpty();
  int size();
  void resetDeck();
 private:
  std::vector<Card> deck;
  int nextCardIndex;
  int seed; //Default is 0, but it can be specified by caller.
  void shuffleDeck();
  void initializeDeck();
  int getRandomValue();
  void swap( int index1, int index2 ); //TODO: Make this a template method
};

#endif

#include "../include/CardDeck.h"

/*class CardDeck {
 public:
  CardDeck();
  CardDeck( int seed );
  Card getNextCard();
  bool isEmpty();
  int size();
  void resetDeck();
 private:
  std::vector<Card> deck;
  int seed; //Default is 0, but it can be specified by caller.
  void shuffleDeck();
  void initializeDeck();
  int getRandomValue();
  void swap( int index1, int index2 ); //TODO: Make this a template method
  };*/


CardDeck::CardDeck():seed{0},deck{52},nextCardIndex{0}{}
CardDeck::CardDeck(int seed):seed{seed},deck{52},nextCardIndex{0}{}

Card CardDeck::getNextCard()
{
  return deck[nextCardIndex++];
}

bool CardDeck::isEmpty()
{
  return deck.size() == nextCardIndex;
}

int CardDeck::size()
{
  return deck.size() - nextCardIndex;
}

void CardDeck::resetDeck()
{
  initializeDeck();
  shuffleDeck();
}

void CardDeck::shuffleDeck()
{
  
}

void CardDeck::initializeDeck()
{
  int deckIndex = 0;

  while( deckIndex < deck.size() ) {
    
    Suit s = static_cast<Suit>(deckIndex%13);
    Type t = static_cast<Type>(deckIndex - 13*(deckIndex%13));
    deck[deckIndex++] = Card{s,t};
  }
}

#include "../include/CardDeck.h"
#include <cstdlib>
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


CardDeck::CardDeck():seed{0},deck{52},nextCardIndex{0}
{
  resetDeck();
}
CardDeck::CardDeck(int seed):seed{seed*3},deck{52},nextCardIndex{0}
{
  resetDeck();
}

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
  srand( seed );
  for( int i = 0; i < 50; ++i ) { //Do 50 times
    for( int j = 0; j < deck.size(); ++j ) { //Go through all cards
      swap(j, getRandomValue());
    }
  }
}

int CardDeck::getRandomValue()
{
  return rand() % 52;
}

void CardDeck::swap( int index1, int index2 )
{
  auto tmp = deck[index1];
  deck[index1] = deck[index2];
  deck[index2] = tmp;
}

void CardDeck::initializeDeck()
{
  int deckIndex = 0;

  while( deckIndex < deck.size() ) {

    Suit s = static_cast<Suit>(deckIndex/13);
    Type t = static_cast<Type>(deckIndex%13);
    deck[deckIndex++] = Card{s,t};
  }
}

std::string cardToString( const Card &c )
{
  std::string type{type_translated_to_text[static_cast<int>(c.type)]};
  std::string suit{suit_translated_to_text[static_cast<int>(c.suit)]};
  return type + " of " + suit;;
}

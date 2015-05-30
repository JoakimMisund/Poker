#include "../include/CardDeck.h"
#include <cstdlib>

CardDeck::CardDeck():deck{52},nextCardIndex{0},seed{0}
{
  resetDeck();
}
CardDeck::CardDeck(int seed):deck{52},nextCardIndex{0},seed{seed}
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
  for( unsigned int i = 0; i < 50; ++i ) { //Do 50 times
    for( unsigned int j = 0; j < deck.size(); ++j ) { //Go through all cards
      swap(j, getRandomValue());
    }
  }
  seed = seed * 13;
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
  unsigned int deckIndex = 0;
  nextCardIndex = 0;

  while( deckIndex < deck.size() ) {

    Suit s = static_cast<Suit>(deckIndex/13);
    Type t = static_cast<Type>(deckIndex%13);
    deck[deckIndex++] = Card{s,t};
  }
}

static const char* type_translated_to_text[] = {"two","three","four","five","six","seven","eight","nine","ten","jack","queen","king","ace"};
static const char* suit_translated_to_text[] = {"heart","club","spade","diamond"};

std::string cardToString( const Card &c )
{
  std::string type{type_translated_to_text[static_cast<int>(c.type)]};
  std::string suit{suit_translated_to_text[static_cast<int>(c.suit)]};
  return type + " of " + suit;;
}

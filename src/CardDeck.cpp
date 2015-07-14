#include "../include/CardDeck.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

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
  srand( time(NULL) );
  for( unsigned int i = 0; i < 50; ++i ) { //Do 50 times
    for( unsigned int j = 0; j < deck.size(); ++j ) { //Go through all cards
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
  unsigned int deckIndex = 0;
  nextCardIndex = 0;

  while( deckIndex < deck.size() ) {

    Suit s = static_cast<Suit>(deckIndex/13);
    Type t = static_cast<Type>(deckIndex%13);
    deck[deckIndex++] = Card{s,t};
  }
}

static const std::string type_translated_to_text[] = {"two","three","four","five","six","seven","eight","nine","ten","jack","queen","king","ace"};
static const std::string suit_translated_to_text[] = {"heart","club","spade","diamond"};

static const std::string type_translated_to_shorthand[] = {"2","3","4","5","6","7","8","9","10","J","Q","K","A"};

std::string cardToString( const Card &c )
{
  Type t = c.type;
  Suit s = c.suit;

  std::string card;
  
  card =  type_translated_to_shorthand[static_cast<int>(t)];
  
  switch(s) {
  case Suit::HEART: card +=  "\033[0;31m\u2665\033[0;0m";
    break;
  case Suit::CLUB: card += "\033[0;30m\u2663\033[0;0m";
    break;
  case Suit::DIAMOND: card += "\033[0;31m\u2666\033[0;0m";
    break;
  case Suit::SPADE: card += "\033[0;30m\u2660\033[0;0m";
    break;
  }

  return card;
}

void printCard( const Card &c )
{
  Type t = c.type;
  Suit s = c.suit;
  
  std::cout << type_translated_to_shorthand[static_cast<int>(t)];
  
  switch(s) {
  case Suit::HEART: std::cout << "\033[0;31m\u2665\033[0;0m";
    break;
  case Suit::CLUB: std::cout << "\033[0;30m\u2663\033[0;0m";
      break;
  case Suit::DIAMOND: std::cout << "\033[0;31m\u2666\033[0;0m";
    break;
  case Suit::SPADE: std::cout << "\033[0;30m\u2660\033[0;0m";
    break;
  default:
    std::cout << "error!! printCard\n" << " type: " << t << " suit: " << s << "\n";
  }
}

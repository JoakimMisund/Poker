#include "../include/CardDeck.h"
#include "../include/Table.h"
#include <iostream>
#include <ctime>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>
#include "../include/Utils.h"


void test_getHandStrength();
void test_findBestCombination();



int main( int argc, char *argv[] )
{
#ifdef DEBUG
  test_findBestCombination();
  test_getHandStrength();
#endif
  //construct table
  Table t;
  User user(15000);
  t.registerUser( &user, 0 ); //add a User
  t.registerUser( nullptr, 1 ); //add a computer
  t.registerUser( nullptr, 2 ); //add a computer
  t.registerUser( nullptr, 3 ); //add a computer
  t.registerUser( nullptr, 4 ); //add a computer
  t.registerUser( nullptr, 5 ); //add a computer
  t.registerUser( nullptr, 6 ); //add a computer
  t.registerUser( nullptr, 7 ); //add a computer
  t.runTable();
  return 0;
}

void test_getHandStrength()
{
  CardDeck deck(time(NULL));
  std::string strengths[]{"high card","pair","two pairs", "three of a kind", "straight", "flush", "full house", "four of a kind", "straight flush" };

  //handStrength getHandStrength( std::vector<Card> &hand, Type &t1, Type &t2 )
  std::vector<Card> hand;
  
  for( int i = 0; i < 10; ++i ) { //run test 10 times
    hand.clear();
    deck.resetDeck();
    for( int j = 0; j < 5; ++j ) { //get 5 random cards
      Card c = deck.getNextCard();
      hand.push_back(c);
    }

    print_hand(hand);
    Type t1,t2;
    handStrength strength = getHandStrength( hand, t1, t2);

    std::cout << strengths[strength] << "\n";
    if( strength == handStrength::STRAIGHT_FLUSH ) break;

  }
}

void test_findBestCombination()
{
  //std::vector<Card> findBestCombination( std::vector<Card> cards )
  //input 7 cards, output: 5 cards (the ideal combination)

  CardDeck deck(time(NULL));
  std::vector<Card> possibleCards;
 

  for( int i = 0; i < 7; ++i ) { //add 7 cards
    possibleCards.push_back( deck.getNextCard() );
  }

  std::cout << "Cards: ";
  for_each( begin(possibleCards), end(possibleCards), printCard );
 
  std::vector<Card> bestHand = findBestCombination( possibleCards );
  
  std::cout << "\nBest hand: ";
  for_each( begin(bestHand), end(bestHand), printCard );
  std::cout << "\n";
}

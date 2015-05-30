#include "../include/CardDeck.h"
#include "../include/Table.h"
#include <iostream>
#include <ctime>
#include <map>

class Table;

void runTable( Table &t );

int main( int argc, char *argv[] )
{
  CardDeck deck(time(NULL));

  std::map<std::string,int> seen_cards;
  
  for( int k = 0; k < 10; ++k) {
    deck.resetDeck();

    for( int i = 0; i < 52; ++i ) {
      Card next_card = deck.getNextCard();
      //std::cout << cardToString( next_card ) << "\n";
      seen_cards[cardToString(next_card)] = 1;
    }

    std::cout << "Nr of distinc cards = "  << seen_cards.size() << "\n";
  }

  //construct table
  Table t;
  User user(15000);
  t.registerUser( &user, 0 );
  t.registerUser( nullptr, 1 );
  //run the table
  runTable(t);
  
  return 0;
}

void runTable( Table &t )
{
  CardDeck deck(time(NULL));
  int rounds = 4;
  while(rounds-- > 0) {

    deck.resetDeck();
    //Give all players 2 cards
    auto players = t.getPlayers();
    for( auto &player: players ) {
      player.setFirstCard( deck.getNextCard() );
    }
    for( auto &player: players ) {
      player.setSecondCard( deck.getNextCard() );

      std::cout << "Player " << player.getTablePosition() << ":" <<  
	cardToString(player.getFirstCard()) << " " << cardToString(player.getSecondCard()) << "\n";
    }

    int numCardsOnBoard = t.getNumCards();
    switch( numCardsOnBoard ) {
    case 0: //pre flopp
    case 3: //flopp
    case 4: //turn
    case 5: //river
    default: ;//defuq
    }

    std::cout << "\n";
  } 
}






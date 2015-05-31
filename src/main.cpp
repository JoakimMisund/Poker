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
  unsigned short dealerPosition = 0;
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

    Player *smallBlind = f.findSmallBlind( dealerPosition );
    Player *bigBlind = findBigBlind( dealerPosition );


    while(1) { //run hand
      unsigned int bets[MAX_NR_PLAYERS]{0};
      bool folded[MAX_NR_PLAYERS]{false};
      
      Player *playerToAct = nullptr;
      Player *lastInRound = nullptr;
      //have to find next player to act
      if( t.getNumCards() == 0 ) { //flopp
	//collect blinds
	bets[smallBlind.getTablePosition()] = 50;
	bets[bigBlind.getTablePosition()] = 100;
	smallBlind.reduceStackSize( 50 );
	bigBlind.reduceStackSize( 100 );
	//done collecting blinds
	playerToAct = t.getPlayerAfter( bigBlind );
	lastInRound = bigBlind;
      } else {
	playerToAct = t.getPlayerAfter( bigBlind );
	lastInRound = bigBlind;
      }

      Action actionToMatch{ActionType::CHECK,0};
      while( playerToAct != lastInRound ) { //If the next player to act is the last in the round, the round is over
	
	Action a = playerToAct->promptForAction( actionToMatch );
	//check if valid action

	if( a.action == ActionType::BET ||
	    a.action == ActionType::RAISE ) { //new lastInRound
	  lastInRound = playerToAct;
	  bets[playerToAct->getTablePosition()] += a.amount;
	  actionToMatch.action = a.action;
	  actionToMatch.amount = bets[playerToAct->getTablePosition()];
	} else if ( a.action == ActionType::FOLD ) {
	  folded[playerToAct->getTablePosition] = true;
	} else {
	  bets[playerToAct->getTablePosition()] += actionToMatch.amount;
	}
	
	std::cout << "player " << playerToAct->getTablePosition() << ": " << a.action << " " << a.amount << "\n";
	
	playerToAct = t.getPlayerAfter( playerToAct );
      }
      
      if( t.getNumCards() == 0 ) {
	t.addCardToBoard();
      }

      //Find winner
    }    
    std::cout << "\n";
  } 
}


void incrementTablePosition( unsigned int &index, int nr )
{
  index = (index + nr) % MAX_NR_PLAYERS;
}


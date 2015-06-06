#include "../include/CardDeck.h"
#include "../include/Table.h"
#include <iostream>
#include <ctime>
#include <map>
#include <vector>
#include <algorithm>

class Table;

void runTable( Table &t );
Player* findWinner( Table &t );

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

    Player *smallBlind = t.findSmallBlind( dealerPosition );
    Player *bigBlind = t.findBigBlind( dealerPosition );


    while(1) { //run hand
      unsigned int bets[MAX_NR_PLAYERS]{0}; //bets for each round
      bool folded[MAX_NR_PLAYERS]{false}; //Player folded if their position is true

      Player *playerToAct = nullptr;
      Player *lastInRound = nullptr;
      //have to find next player to act
      if( t.getNumCards() == 0 ) { //flopp
	//collect blinds
	bets[smallBlind->getTablePosition()] = 50; //Blind
	bets[bigBlind->getTablePosition()] = 100; //blind
	smallBlind->reduceStackSize( 50 ); //reduce stack size
	bigBlind->reduceStackSize( 100 ); //reduce stack size
	//done collecting blinds
	playerToAct = t.getPlayerAfter( bigBlind );
	lastInRound = playerToAct; //if
      } else {
	playerToAct = t.getPlayerAfter( bigBlind );
	lastInRound = playerToAct;
      }

      Action actionToMatch{ActionType::CHECK,0};
      do {

	unsigned int tablePosition = playerToAct->getTablePosition();
	//check if player has folded!
	if( !folded[tablePosition] ) {
	  Action a = playerToAct->promptForAction( actionToMatch );

	  //check if valid action
	  if( a.action == ActionType::BET ||
	      a.action == ActionType::RAISE ) { //new lastInRound
	    lastInRound = playerToAct;
	    bets[tablePosition] += a.amount;
	    playerToAct->reduceStackSize( a.amount );
	    actionToMatch.action = a.action;
	    actionToMatch.amount = bets[tablePosition];
	  } else if ( a.action == ActionType::FOLD ) {
	    folded[tablePosition] = true;
	  } else {
	    bets[tablePosition] += actionToMatch.amount;
	  }

	  std::cout << "player " << tablePosition << ": " << a.action << " " << a.amount << "\n";
	}

	playerToAct = t.getPlayerAfter( playerToAct );

      } while ( playerToAct != lastInRound ); //If the next player to act is the last in the round, the round is over

      unsigned int numCardsOnTable = t.getNumCards();
      Player* winner = nullptr;
      switch( numCardsOnTable ) {

      case 0:	 //pre-flop
	t.addCardToBoard(deck.getNextCard());
	t.addCardToBoard(deck.getNextCard());
	t.addCardToBoard(deck.getNextCard());
	break;
      case 3:
      case 4:
	t.addCardToBoard(deck.getNextCard());
	break;
      case 5: //end
	winner = findWinner( t );
	break;
      default:
	std::cout << "nuym cards: " << numCardsOnTable <<  "\n";
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

enum handStrength{ HIGH_CARD = 0, PAIR = 1, TWO_PAIRS = 2, THREE_OF_A_KIND = 3, STRAIGHT = 4, FLUSH = 5, FULL_HOUSE = 6, FOUR_OF_A_KIND = 7, STRAIGHT_FLUSH = 8 };

handStrength getHandStrength( std::vector<Card> &hand, Type &t1, Type &t2 )
{
  std::sort( begin(hand), end(hand), []( Card c1, Card c2 ) {
      return c1.type < c2.type;
    } );

  //check is straight or flush or both
  Type highCard = Type::TWO;
  Type t = hand[0].type;
  Suit s = hand[0].suit;
  bool straight = true;
  bool flush = true;

  for( unsigned int i = 1; i < hand.size(); ++i ) { //check flush and straight
    Card curr = hand[i];

    if( curr.type != t+1 && curr.suit != s ) { //no straight or flush
      straight = flush = false;
      break;
    } else if( curr.type != t+1 || curr.suit != s ) { //Either not a straight or a flush

      if( curr.type != t+1 ) { //not a straight
	straight = false;
      } else { //not a flush
	flush = false;
      }
    }

    highCard = (highCard < curr.type) ? curr.type:highCard;
  }

  if( straight && flush ) {
    t1 = highCard;
    return STRAIGHT_FLUSH;
  } else if( straight ) {
    t1 = highCard;
    return STRAIGHT;
  } else if( flush ) {
    t1 = highCard;
    return STRAIGHT_FLUSH;
  }

  //find max of a kind
  unsigned short max = 0;
  t = Type::TWO;
  for( unsigned int i = 0; i < hand.size(); ++i ) {
    Card c = hand[i];
    unsigned short count = 0;
    for( unsigned int j = i+1; j < hand.size(); ++j ) {
      if( hand[j].type == c.type ) count++;
    }

    if( count == 4 ) {
      t1 = c.type;
      return FOUR_OF_A_KIND;
    } else if( count == 3 && max == 2 ) {
      t1 = c.type;
      t2 = t;
      return FULL_HOUSE;
    } else if( count == 2 && max == 3 ) {
      t2 = c.type;
      t1 = t;
      return FULL_HOUSE;
    } else if( count == 2 && max == 2 ) {
      t1 = c.type;
      t2 = t;
      return TWO_PAIRS;
    }

    if( count > max || ( count == max && c.type > t ) ) { //new best
      max = count;
      t = c.type;
    }
  }

  t1 = t;

  if( max == 1 ) {
    return HIGH_CARD;
  } else if( max == 2 ) {
    return PAIR;
  } else if( max == 3 ) {
    return THREE_OF_A_KIND;
  }

  std::cout << "Should never be here!!!!\n";
  return HIGH_CARD;
}

bool compareHands( std::vector<Card> &hand1, std::vector<Card> &hand2 )
{
  Type h1t1;
  Type h1t2;
  handStrength h1 = getHandStrength( hand1, h1t1, h1t2 );

  //  handStrength h2 = HIGH_CARD;
  return 1;

}

Player* findWinner( Table &t )
{

  return nullptr;
}

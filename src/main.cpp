#include "../include/CardDeck.h"
#include "../include/Table.h"
#include <iostream>
#include <ctime>
#include <map>
#include <vector>
#include <algorithm>

class Table;

enum handStrength{ HIGH_CARD = 0, PAIR = 1, TWO_PAIRS = 2, THREE_OF_A_KIND = 3, STRAIGHT = 4, FLUSH = 5, FULL_HOUSE = 6, FOUR_OF_A_KIND = 7, STRAIGHT_FLUSH = 8 };

void runTable( Table &t );
Player* findWinner( Table &t );
/*
enum Suit{ HEART, CLUB, SPADE, DIAMOND };
enum Type{ TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE };
*/

void print_hand( std::vector<Card> &hand )
{

  char* types[]{"two","three","four","five","six","seven","eight","nine","ten","jack","queen","king","ace"};
  char* suits[]{"heart","club","spade","diamond"};
  for( Card &c : hand ) {
    std::cout << types[c.type] << " of " << suits[c.suit] << " ";
  }
  std::cout << "\n";
}
handStrength getHandStrength( std::vector<Card> &hand, Type &t1, Type &t2 );

void test_getHandStrength()
{
  CardDeck deck(time(NULL));
  char* strengths[]{"high card","pair","two pairs", "three of a kind", "straight", "flush", "full house", "four of a kind", "straight flush" };

  //handStrength getHandStrength( std::vector<Card> &hand, Type &t1, Type &t2 )
  std::vector<Card> hand;
  
  for( int i = 0; i < 1000; ++i ) { //run test 10 times
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
    if( strength == handStrength::FLUSH || strength == handStrength::STRAIGHT ) break;

  }
}

int main( int argc, char *argv[] )
{

  test_getHandStrength();
  return 1;
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
  /*  Table t;
  User user(15000);
  t.registerUser( &user, 0 );
  t.registerUser( nullptr, 1 );
  //run the table
  runTable(t);
  */
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
  std::cout << t << "\n";

  for( unsigned int i = 1; i < hand.size(); ++i ) { //check flush and straight
    Card curr = hand[i];
    std::cout << curr.type << "\n";
 
    if( curr.type != t+1 ) { //not a straight
      straight = false;
    }
    if( curr.suit != s ) { //not a flush
      flush = false;
    }
    t = curr.type;


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
    return FLUSH;
  }

  //find max of a kind
  unsigned short max = 0;
  t = Type::TWO;
  std::cout << "hand size: " << hand.size() << "\n";
  for( unsigned int i = 0; i < hand.size(); ++i ) { //go through all cards
    Card c = hand[i];
    unsigned short count = 1; //how many do i have of this kind?
    for( unsigned int j = i+1; j < hand.size(); ++j ) {
      if( hand[j].type == c.type ) count++;
    }
    std::cout << "Of this kind: " << count << "\n";

    if( count == 4 ) {
      t1 = c.type;
      return FOUR_OF_A_KIND;
    } else if( count == 3 && max == 2 ) {
      t1 = c.type;
      t2 = t;
      return FULL_HOUSE;
    } else if( count == 2 && max == 3 && t != c.type ) {
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

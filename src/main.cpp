#include "../include/CardDeck.h"
#include "../include/Table.h"
#include <iostream>
#include <ctime>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>

class Table;

enum handStrength{ HIGH_CARD = 0, PAIR = 1, TWO_PAIRS = 2, THREE_OF_A_KIND = 3, STRAIGHT = 4, FLUSH = 5, FULL_HOUSE = 6, FOUR_OF_A_KIND = 7, STRAIGHT_FLUSH = 8 };

void runTable( Table &t );
std::vector<Player*> findWinners( Table &t, std::vector<Player*> &players );
std::vector<Card> findBestCombination( std::vector<Card> &cards );

void print_hand( std::vector<Card> &hand );
handStrength getHandStrength( std::vector<Card> hand, Type &t1, Type &t2 );

void test_getHandStrength();
void test_findBestCombination();



int main( int argc, char *argv[] )
{
  if( DEBUG ) {
    test_findBestCombination();
    test_getHandStrength();
  }
  //construct table
    Table t;
  User user(15000);
  t.registerUser( &user, 0 ); //add a user
  t.registerUser( nullptr, 1 ); //add a computer
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
    std::vector<Player> &players = t.getPlayers();
    for( auto &player: players ) {
      player.setFirstCard( deck.getNextCard() );
    }
    for( auto &player: players ) {
      player.setSecondCard( deck.getNextCard() );
      player.folded = false;

      std::cout << "Player " << player.getTablePosition() << ": ";
      printCard( player.getFirstCard());
      printCard( player.getSecondCard());
      std::cout << "\n";
    }

    Player *smallBlind = t.findSmallBlind( dealerPosition );
    Player *bigBlind = t.findBigBlind( dealerPosition );

    bool folded[MAX_NR_PLAYERS]{false}; //Player folded if their position is true
    int nrPlayersLeft = players.size();
    while(1) { //run hand
      unsigned int bets[MAX_NR_PLAYERS]{0}; //bets for each round

      int nrPlayersLeft = players.size();

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

      //enum ActionType { FOLD, BET, CALL, RAISE, CHECK };
      std::string actions[]{"FOLDE", "BET", "CALL", "RAISE", "CHECK"};
      Action actionToMatch{ActionType::FOLD,0};
      do {
	std::cout << "action to match: " << actions[actionToMatch.action] << "\n";
	unsigned int tablePosition = playerToAct->getTablePosition();
	//check if player has folded!
	if( !folded[tablePosition] ) { //player has not folded
	  Action a = playerToAct->promptForAction( actionToMatch );

	  //check if valid action
	  if( a.action == ActionType::BET ||
	      a.action == ActionType::RAISE ) { //new lastInRound
	    lastInRound = playerToAct;
	    int outstandingAmount = bets[tablePosition];
	    bets[tablePosition] = a.amount;
	    playerToAct->reduceStackSize( a.amount - outstandingAmount );
	    actionToMatch.action = a.action;
	    actionToMatch.amount = bets[tablePosition];
	  } else if ( a.action == ActionType::CALL) {

	    int outstandingAmount = bets[tablePosition];
	    bets[tablePosition] = actionToMatch.amount;
	    playerToAct->reduceStackSize( actionToMatch.amount - outstandingAmount );
	    
	  } else if ( a.action == ActionType::FOLD ) {
	    folded[tablePosition] = true;
	    playerToAct->folded = true;
	    nrPlayersLeft--;
	  } else if( a.action == ActionType::CHECK ) {
	    if( bets[tablePosition] < actionToMatch.amount ) {
	      folded[tablePosition] = true;
	    }
	  }
	  

	  std::cout << "player " << tablePosition << ": " << actions[a.action] << " " << a.amount << "\n";
	}

	playerToAct = t.getPlayerAfter( playerToAct );

      } while ( playerToAct != lastInRound ); //If the next player to act is the last in the round, the round is over


      unsigned int numCardsOnTable = t.getNumCards();
      //todo numcardsontable
      std::vector<Player*> winners;
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
	//TODO send with list of players that are still in the game
	winners = findWinners( t );

	std::cout << "winner(s): ";
	for( Player *p : winners ) {
	  std::cout << p->getTablePosition() << "\n";
	}
	return;
	break;
      default:
	std::cout << "nuym cards: " << numCardsOnTable <<  "\n";
      }
      
      std::vector<Card> cardsOnTheTable = t.getCardsOnBoard();
      for_each( begin(cardsOnTheTable), end(cardsOnTheTable), printCard );

      //Find winner
    }
    std::cout << "\n";
  }
}


void incrementTablePosition( unsigned int &index, int nr )
{
  index = (index + nr) % MAX_NR_PLAYERS;
}

//seems to be done
handStrength getHandStrength( std::vector<Card> hand, Type &t1, Type &t2 )
{
  std::sort( begin(hand), end(hand), []( Card c1, Card c2 ) {
      return c1.type < c2.type;
    } );

  t1 = t2 = Type::TWO; //added to make the callers job easier. Does not have to check what strength

  //check is straight or flush or both
  Type highCard = Type::TWO;
  Type t = hand[0].type;
  Suit s = hand[0].suit;
  bool straight = true;
  bool flush = true;

  for( unsigned int i = 1; i < hand.size(); ++i ) { //check flush and straight
    Card curr = hand[i];
 
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
  for( unsigned int i = 0; i < hand.size(); ++i ) { //go through all cards
    Card c = hand[i];
    unsigned short count = 1; //how many do i have of this kind?
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

//1 if hand1 is better than hand2, -1 if hand2 is better, equal => 0; 
int compareHands( std::vector<Card> hand1, std::vector<Card> hand2 )
{
  Type h1t1, h2t1;
  Type h1t2, h2t2;
  handStrength h1 = getHandStrength( hand1, h1t1, h1t2 );
  handStrength h2 = getHandStrength( hand2, h2t1, h2t2 );

  if( h1 != h2 ) {
    return (h1 > h2) ? 1:-1;
  }
  

  if( h1t1 != h2t1 ) {
    return (h1t1 > h2t1) ? 1:-1;
  }
  if( h1t2 != h2t2 ) {
    return (h1t2 > h2t2) ? 1:-1;
  }

  //Highest card desides.
  std::sort( begin(hand1), end(hand1), []( Card c1, Card c2 ) {
      return c1.type < c2.type;
    } );
  std::sort( begin(hand2), end(hand2), []( Card c1, Card c2 ) {
      return c1.type < c2.type;
    } );

  for( int i = hand1.size()-1; i >= 0; --i ) {
    if( hand1[i].type == h1t1 || hand1[i].type == h1t2) continue;
    if( hand1[i].type > hand2[i].type ) return 1;
    else if( hand1[i].type < hand2[i].type ) return -1;
  }

  
  return 0;

}

//find best hand of 5 from 7 cards.
std::vector<Card> findBestCombination( std::vector<Card> &cards )
{
  assert( cards.size() == 7 );
  
  std::vector<Card> currentHand;
  std::vector<Card> bestHand;

  //i is the index of a card to exclude
  for( unsigned int i = 0; i < cards.size(); ++i ) {
    //j is the index of a card to exclude.
    for( unsigned int j = i+1; j < cards.size(); ++j ) {
    
      currentHand.clear();
      //add cards
      for( unsigned int k = 0; k < cards.size() && currentHand.size() != 5; ++k ) {
	if( k != i && k != j ) {
	  currentHand.push_back(cards[k]);
	}
      }

      if( bestHand.size() == 0 ) {
	bestHand = currentHand;
      } else {
	
	std::cout << "Best so far: ";
	for_each( begin(bestHand), end(bestHand), printCard );
	std::cout << " Challenger: ";
	for_each( begin(currentHand), end(currentHand), printCard );
	std::cout << "\n";
	
	if( compareHands( currentHand, bestHand ) == 1 ) {
	  bestHand = currentHand;
	}
      }
      
    }
  }

  return bestHand;

}

std::vector<Player*> findWinners( Table &t, std::vector<Player*> &players );
{
  //have to generate the best hand of all the players, then have an elimination tournament.
  std::vector<Player*> currentBest;
  std::vector<Card> bestHand;

  for( Player &p : players ) {
  
    std::cout << "Player: " << p.getTablePosition() << "\n";
    unsigned int tablePosition = p.getTablePosition();
    //todo find best hand, compare to next one.
    std::vector<Card> possibleCards = t.getCardsOnBoard();
    possibleCards.push_back( p.getFirstCard() );
    possibleCards.push_back( p.getSecondCard() );
    

    std::vector<Card> hand = findBestCombination( possibleCards );

    if( bestHand.size() == 0 ) {
      currentBest.push_back(&p);
      bestHand = hand;
    } else {

      int compareResult = compareHands(hand, bestHand);
      if( compareResult == 1 ) {
        currentBest.clear();
        currentBest.push_back(&p);
        bestHand = hand;
      } else if ( compareResult == 0 ) { //new shared best
        currentBest.push_back(&p);
      }
    }
    
  }
  std::cout << "winning hand: ";

  for_each( begin(bestHand), end(bestHand), printCard );
  std::cout << "\n";
  
  return currentBest;
}



void print_hand( std::vector<Card> &hand )
{

  std::string types[]{"two","three","four","five","six","seven","eight","nine","ten","jack","queen","king","ace"};
  std::string suits[]{"heart","club","spade","diamond"};
  for( Card &c : hand ) {
    std::cout << types[c.type] << " of " << suits[c.suit] << " ";
  }

  std::cout << "\n";
  for_each( begin(hand), end(hand), 
	    [](const Card &c) { printCard(c); std::cout << " "; } );
  std::cout << "\n";
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

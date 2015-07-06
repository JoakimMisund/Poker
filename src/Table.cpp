#include "../include/Table.h"
#include <ctime>
#include <iostream>
#include <algorithm>
#include <array>

/*class Table {
  static int nextTableId;

  public:
  Table();
  int getTableId();
  unsigned int setPotSize( unsigned int value);
  unsigned int getPotSize();
  std::vector<Player*> getPlayers();
  void addCardToBoard( Card c );
  bool registerUser( User *user, int tablePosition );

  private:
  std::vector<Player*> players;
  std::vector<Card> board;

  unsigned int tableId;
  unsigned int potSize;
  };
*/


Table::Table():players{0},board{0},tableId{nextTableId++}{}
unsigned int Table::nextTableId = 0;

int Table::getTableId() { return tableId; }

std::vector<Player*> Table::getPlayers()
{
  std::vector<Player*> players_p;

  for( Player &p : players ) {
    players_p.push_back( &p );
  }
  
  return players_p;
  
}


std::vector<Player*> getPlayingPlayers( std::vector<Player*> &players )
{
  std::vector<Player*> activePlayers (players.size());
  auto it = std::copy_if( begin(players), end(players), begin(activePlayers),
			  [](Player *p) { return p->getState() == ACTIVE; });
  activePlayers.resize(std::distance(begin(activePlayers), it));
  return activePlayers;
}

void Table::giveTwoCardsToPlayers( std::vector<Player*> players )
{
  deck.resetDeck();
  for( auto player: players ) {
    player->setFirstCard( deck.getNextCard() );
  }
  for( auto player: players ) {
    player->setSecondCard( deck.getNextCard() );

    std::cout << "Player " << player->getTablePosition() << ": ";
    printCard( player->getFirstCard());
    printCard( player->getSecondCard());
    std::cout << "\n";
  }
}

void Table::runHand( std::vector<Player*> playersInHand )
{
  
  unsigned int dealerPosition = 0;
    
  int nrPlayers = playersInHand.size();
  if( nrPlayers < 2 ) {
    std::cout << "Two few players!\n";
    return;
  }
    
  giveTwoCardsToPlayers( playersInHand );
    
  Player *smallBlind = findSmallBlind( dealerPosition, playersInHand );
  Player *bigBlind = findBigBlind( dealerPosition, playersInHand );
   
  unsigned int potSize = 0;
    
  while( 1 ) {
    std::array<int,8> bets {0,0,0,0,0,0,0,0}; //bets of players
      
    Player *playerToAct = nullptr;
    Player *lastInRound = nullptr;
     
    playerToAct = getPlayerAfter(bigBlind, playersInHand);
    lastInRound = playerToAct;

    if( getNumCards() == 0 ) { //preflopp
      bets[smallBlind->getTablePosition()] = 50; //Blind
      bets[bigBlind->getTablePosition()] = 100; //blind
      smallBlind->reduceStackSize( 50 ); //reduce stack size
      bigBlind->reduceStackSize( 100 ); //reduce stack size
    }

    Action actionToMatch{CHECK,0};
      
    do {
      unsigned int tablePosition = playerToAct->getTablePosition();
      Action a = playerToAct->promptForAction( actionToMatch );
	
      if( a.action == ActionType::BET ||
	  a.action == ActionType::RAISE ) {

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

	playersInHand.erase(std::remove(begin(playersInHand),end(playersInHand), playerToAct),
			    end(playersInHand));
      }

      playerToAct = getPlayerAfter( playerToAct, playersInHand );

    } while (lastInRound != playerToAct);

    unsigned int numCardsOnTable = getNumCards();
    std::vector<Player*> winners;
    //todo numcardsontable
    switch( numCardsOnTable ) {

    case 0:	 //pre-flop
      addCardToBoard(deck.getNextCard());
      addCardToBoard(deck.getNextCard());
      addCardToBoard(deck.getNextCard());
      break;
    case 3:
    case 4:
      addCardToBoard(deck.getNextCard());
      break;
    case 5: //end
      //TODO send with list of players that are still in the game
      winners = findWinners( playersInHand );

      std::cout << "winner(s): ";
      for( Player *p : winners ) {
	std::cout << "Player " << p->getTablePosition() << " won! \n";
      }
      return;
    default:
      std::cout << "nuym cards: " << numCardsOnTable <<  "\n";
    }
      
    std::vector<Card> cardsOnTheTable = getCardsOnBoard();
    for_each( begin(cardsOnTheTable), end(cardsOnTheTable), printCard );
  }
}

void Table::runTable()
{
  while( 1 ) {
    std::vector<Player*> players_ptrs(0);
    for( auto &p : players ) players_ptrs.push_back(&p);
    std::vector<Player*> playersInHand = getPlayingPlayers( players_ptrs );

    runHand( playersInHand );
    
    board.clear();
    deck.resetDeck();
    
  }
}
void Table::addCardToBoard( Card c ) { board.push_back(c); }
std::vector<Card> &Table::getCardsOnBoard() { return board; }
unsigned int Table::getNumCards() { return board.size(); }

bool Table::registerUser( User *user, unsigned int tablePosition )
{

  if( tablePosition < 0 || tablePosition >= MAX_NR_PLAYERS ) {
    return false;
  }

  //check is a player allready present in position
  for( auto p : players ) {
    if( p.getTablePosition() == tablePosition ) return false;
  }

  Player newPlayer( user, 15000, tablePosition );
  players.push_back(newPlayer);

  std::sort( begin(players), end(players), [](Player p1, Player p2) {
      return p1.getTablePosition() < p2.getTablePosition();
    });

  return true;
}

Player* Table::getPlayerAfter( Player *p, std::vector<Player*> &players )
{
  if( p == nullptr ) {
    return nullptr;
  }

  unsigned int tablePosition = p->getTablePosition();
  Player* after = players[0];
  for( auto p : players ) {
    if( p->getTablePosition() > tablePosition ) after = p;
  }
  return after;
}

Player* Table::getPlayerAtPosition( unsigned int position, std::vector<Player*> &players )
{
  for( auto p : players ) {
    if( p->getTablePosition() == position) return p;
  }
  return nullptr;
}

Player* Table::findSmallBlind( unsigned int dealerPosition, std::vector<Player*> &players )
{
  Player *p = getPlayerAtPosition(dealerPosition, players);
  return getPlayerAfter( p, players );
}
Player* Table::findBigBlind( unsigned int dealerPosition, std::vector<Player*> &players )
{
  return getPlayerAfter( getPlayerAfter( getPlayerAtPosition(dealerPosition, players), players), players );
}


//std::vector<Player*> findWinners( Table &t, std::vector<Player> &players )
std::vector<Player*> Table::findWinners( std::vector<Player*> &players )
{
  //have to generate the best hand of all the players, then have an elimination tournament.
  std::vector<Player*> currentBest;
  std::vector<Card> bestHand;

  for( Player *p : players ) {
  
    std::cout << "Player: " << p->getTablePosition() << "\n";
    unsigned int tablePosition = p->getTablePosition();
    //todo find best hand, compare to next one.
    std::vector<Card> possibleCards = getCardsOnBoard();
    possibleCards.push_back( p->getFirstCard() );
    possibleCards.push_back( p->getSecondCard() );
    

    std::vector<Card> hand = findBestCombination( possibleCards );

    if( bestHand.size() == 0 ) {
      currentBest.push_back(p);
      bestHand = hand;
    } else {

      int compareResult = compareHands(hand, bestHand);
      if( compareResult == 1 ) {
        currentBest.clear();
        currentBest.push_back(p);
        bestHand = hand;
      } else if ( compareResult == 0 ) { //new shared best
        currentBest.push_back(p);
      }
    }
    
  }
  std::cout << "winning hand: ";

  for_each( begin(bestHand), end(bestHand), printCard );
  std::cout << "\n";
  
  return currentBest;
}

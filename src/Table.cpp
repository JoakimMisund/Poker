#include "../include/Table.h"
#include <ctime>
#include <iostream>
#include <algorithm>
#include <array>
#include <thread>
#include <stack>
#include <chrono>
#include <memory>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>


Table::Table():players{0},board{0},tableId{nextTableId++},deck{},dealerPosition{0}{}
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

struct Pot {

  unsigned int size;
  std::vector<Player*> players;

  Pot();
  Pot( unsigned int s, std::vector<Player*> &p);
  void addPlayer(Player *p);
};
void Pot::addPlayer( Player *p )
{
  players.push_back(p);
}
Pot::Pot():size{0},players{0} {}
Pot::Pot( unsigned int s, std::vector<Player*> &p):size{s}
{
  players = p;
}

////////////

unsigned int min_bet( std::array<int,8> &bets )
{
  int min = 0;
  for( int i : bets ) {
    if( min == 0 ) min = i;
    if( min > i && i != 0 ) min = i;
  }
  return min;
}

void Table::runHand( std::vector<Player*> playersInHand )
{

  std::cout << "nrPlayers " << playersInHand.size() << "\n";
  
  int nrPlayers = playersInHand.size();
  if( nrPlayers < 2 ) {
    std::cout << "Two few players!\n";
    return;
  }
    
  giveTwoCardsToPlayers( playersInHand );
    
  Player *smallBlind = findSmallBlind( dealerPosition, playersInHand );
  Player *bigBlind = findBigBlind( dealerPosition, playersInHand );
  
  std::stack<std::shared_ptr<Pot>> pots;
  std::shared_ptr<Pot> startPot( new Pot(0, playersInHand));
  pots.push(startPot);
  std::shared_ptr<Pot> currentPot = startPot;

  std::vector<std::string> log;
  
  while( 1 ) {

    draw(currentPot->size);
    std::for_each( begin(log), end(log), [](std::string s) {std::cout << s << "\n";});

    std::array<int,8> bets {0,0,0,0,0,0,0,0}; //bets of players
      
    Player *playerToAct = nullptr;
    Player *lastInRound = nullptr;
     
    playerToAct = getPlayerAfter(bigBlind, currentPot->players);
    lastInRound = playerToAct;

    Action actionToMatch{CHECK,0};

    if( getNumCards() == 0 ) { //preflopp
      unsigned int sb = (smallBlind->getStackSize() < 50) ? smallBlind->getStackSize():50;
      unsigned int bb = (bigBlind->getStackSize() < 50) ? bigBlind->getStackSize():100;
      bets[smallBlind->getTablePosition()] = sb; //Blind
      bets[bigBlind->getTablePosition()] = bb; //blind
      smallBlind->reduceStackSize( sb ); //reduce stack size
      bigBlind->reduceStackSize( bb ); //reduce stack size
      actionToMatch.action = BET;
      actionToMatch.amount = bb;
    }

      
    do {

      if( playerToAct->getStackSize() == 0 ) continue;
      unsigned int tablePosition = playerToAct->getTablePosition();
      Action a = playerToAct->promptForAction( actionToMatch );

      // enum ActionType { FOLD, BET, CALL, RAISE, CHECK };

      std::string actions[5] = {"folded", "bets", "calls", "raises to", "checks"};

      std::cout << "Player " << playerToAct->getTablePosition() << " " << actions[a.action];
      std::string log_action = "";
      log_action += std::string("Player ") + std::to_string(playerToAct->getTablePosition()) + " " + actions[a.action] + " " + std::to_string(a.amount);
      log.push_back(log_action);

      if( a.action == ActionType::BET || a.action == ActionType::RAISE ) {
	std::cout << " " << a.amount << "!";
      } 
      std::cout << "\n";
      

      if( a.action == ActionType::BET ||
	  a.action == ActionType::RAISE ) {

	lastInRound = playerToAct;
	int outstandingAmount = bets[tablePosition];
	bets[tablePosition] = a.amount;
	playerToAct->reduceStackSize( a.amount - outstandingAmount );
	actionToMatch.action = a.action;
	actionToMatch.amount = bets[tablePosition];

      } else if ( a.action == ActionType::CALL) {

	if( a.amount < actionToMatch.amount ) { //player is all in
	  int outstandingAmount = bets[tablePosition];
	  bets[tablePosition] += a.amount;
	  playerToAct->reduceStackSize( a.amount );
	  
	} else {
	  int outstandingAmount = bets[tablePosition];
	  bets[tablePosition] = actionToMatch.amount;
	  playerToAct->reduceStackSize( actionToMatch.amount - outstandingAmount );
	}
      } else if ( a.action == ActionType::FOLD ) {

	std::stack<std::shared_ptr<Pot>> tmp;
	while(!pots.empty()) {
	  std::shared_ptr<Pot> c = pots.top();
	  pots.pop();
	  
	  c->players.erase(std::remove( begin(c->players), end(c->players), playerToAct), end(c->players));
	  tmp.push(c);
	}
	while(!tmp.empty()) { pots.push( tmp.top() ); tmp.pop(); }
	
      }
      
      playerToAct = getPlayerAfter( playerToAct, currentPot->players );
      draw(currentPot->size);
      std::for_each( begin(log), end(log), [](std::string s) {std::cout << s << "\n";});
      
      std::this_thread::sleep_for( std::chrono::milliseconds{1000} );

    } while (lastInRound != playerToAct);


    // Make this recursive untill all side pots are done.
    int min_value;

    std::for_each(begin(bets), end(bets), []( int i ) { std::cout << " " << i;});
    std::cout << "\n";
    std::cout << *std::max_element( begin(bets), end(bets) ) << "\n";
    std::this_thread::sleep_for( std::chrono::milliseconds{1000} );

    while( (min_value = min_bet( bets )) != 0 ) {

      std::this_thread::sleep_for( std::chrono::milliseconds{1000} );
      if( min_value != *std::max_element( begin(bets), end(bets) ) && currentPot->players.size() != 1 ) {

	std::for_each(begin(bets), end(bets), []( int i ) { std::cout << " " << i;});
	std::cout << "\n";

	//	std::cout << *std::max_element( begin(bets), end(bets) ) << "\n";
	std::this_thread::sleep_for( std::chrono::milliseconds{1000} );

	std::cout << min_value << "...." << *std::max_element( begin(bets), end(bets) ) << "\n";
	//Need new side pot.
	std::shared_ptr<Pot> sidePot( new Pot(0, currentPot->players ) );
	//Remove the players that cant continue.
	for( Player* p: currentPot->players ) {
	  if( bets[p->getTablePosition()] == min_value ) {
	    sidePot->players.erase(std::remove(begin(sidePot->players),end(sidePot->players), p),
				   end(sidePot->players));
	  }
	}

	for( int &bet : bets ) {
	  if( bet >= min_value ) currentPot->size += min_value;
	  bet -= min_value;
	  (bet < 0) ? bet = 0:bet = bet;
	}
      
	pots.push(sidePot);
	currentPot = sidePot;
      } else { //all players has given the same amount.
	for( int bet : bets ) {
	  currentPot->size += bet;
	}
	break;
      }
    }
    
    unsigned int numCardsOnTable = getNumCards();

    if( currentPot->players.size() == 1 ) {
      std::cout << pots.size() << "\n";
      while( !pots.empty() ) {
	currentPot = pots.top();
	pots.pop();

	Player *p = currentPot->players[0];
	std::cout << "Player " << p->getTablePosition() << " won: " << currentPot->size <<  " with: ";
	if( currentPot->players.size() != 1 ) {
	  printCard(p->getFirstCard());
	  printCard(p->getSecondCard());
	}
	std::cout << "\n";
	p->increaseStackSize(currentPot->size);
	
      }
      return;
    }

    std::vector<Player*> winners;
    //todo numcardsontable
    switch( numCardsOnTable ) {

    case 0:	 //pre-flop
      log.push_back("\nFlop\n");
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
      //winners = findWinners( playersInHand );

      while( !pots.empty() ) {
	currentPot = pots.top();
	pots.pop();

	winners = findWinners(currentPot->players);
	std::cout << "winner(s): ";
	for( Player *p : winners ) {
	  std::cout << "Player " << p->getTablePosition() << " won: " << currentPot->size/winners.size() <<  " with: ";
	  printCard(p->getFirstCard());
	  printCard(p->getSecondCard());
	  std::cout << "\n";
	  p->increaseStackSize(currentPot->size/winners.size());
	}
      }
      return;
    default:
      std::cout << "nuym cards: " << numCardsOnTable <<  "\n";
    }
    std::this_thread::sleep_for( std::chrono::milliseconds{1000} );
  }
}

void Table::runTable()
{
  while( 1 ) {
    std::vector<Player*> players_ptrs(0);
    for( auto &p : players ) players_ptrs.push_back(&p);
    std::vector<Player*> playersInHand = getPlayingPlayers( players_ptrs );

    runHand( playersInHand );
    
    dealerPosition = (dealerPosition >= 6) ? 0:dealerPosition+1;
    board.clear();
    deck.resetDeck();
    std::this_thread::sleep_for( std::chrono::milliseconds{7000} );
    
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

  if( newPlayer.getUser() != nullptr) std::cout << newPlayer.getUser()->getSocket() << "\n";

  return true;
}

Player* Table::getPlayerAfter( Player *p, std::vector<Player*> &players )
{
  if( p == nullptr ) {
    return nullptr;
  }

  unsigned int tablePosition = p->getTablePosition();
  Player* after = players[0];
  unsigned int minDist = 100;
  for( auto p : players ) {
    if( p->getTablePosition() > tablePosition && (p->getTablePosition() - p->getTablePosition()) < minDist ) {
      after = p;
      minDist = p->getTablePosition() - p->getTablePosition();
    }
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

Player* Table::getPlayerAtPosition( unsigned int position )
{
  for( auto &p : players ) {
    if( p.getTablePosition() == position) return &p;
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

void Table::sendToAllPlayers(std::string line)
{
  for( Player &p : players ) {
    if( p.getUser() != nullptr ) {
      int sock = p.getUser()->getSocket();

      if( sock > 0 ) {
	send(sock,line.c_str(),line.size(),0);
      }
    }
  }

}

void Table::draw( unsigned int potSize)
{

  std::cout << "\033[2J \033[2;0f";
  sendToAllPlayers("\033[2J \033[2;0f");
  for( int i = 0; i < MAX_NR_PLAYERS; ++i ) {
    
    Player *p = getPlayerAtPosition(i);
    if( p != nullptr ) {

      switch(i) {
      case 0: std::cout << "\033[2;10f Player 0\n\033[12C" << p->getStackSize();
	sendToAllPlayers("\033[2;10f Player 0\n\033[12C");
	break;
      case 1: std::cout << "\033[2;23f Player 1\n\033[25C" << p->getStackSize();
	sendToAllPlayers("\033[2;23f Player 1\n\033[25C");
	break;
      case 2: std::cout << "\033[2;40f Player 2\n\033[42C" << p->getStackSize();
	sendToAllPlayers("\033[2;40f Player 2\n\033[42C");
	break;
      case 3: std::cout << "\033[7;45f Player 3\n\033[47C" << p->getStackSize();
	sendToAllPlayers("\033[7;45f Player 3\n\033[47C");
	break;
      case 4: std::cout << "\033[12;40f Player 4\n\033[42C" << p->getStackSize();
	sendToAllPlayers("\033[12;40f Player 4\n\033[42C");
	break;
      case 5: std::cout << "\033[12;23f Player 5\n\033[25C" << p->getStackSize();
	sendToAllPlayers("\033[12;23f Player 5\n\033[25C");
	break;
      case 6: std::cout << "\033[12;10f Player 6\n\033[12C" << p->getStackSize();
	sendToAllPlayers("\033[12;10f Player 6\n\033[12C");
	break;
      case 7: std::cout << "\033[7;0f Player 7\n\033[2C" << p->getStackSize();
	sendToAllPlayers("\033[7;0f Player 7\n\033[2C");
	break;
      }
      std::cout << "\033[1B \033[10DCards:";
      sendToAllPlayers("\033[1B \033[5DCards:");
      //      sendToAllPlayers(cardToString(p->getFirstCard()) + cardToString(p->getSecondCard()));
      p->drawCards();
      for( Player &opponent : players ) {
	if( &opponent != p && opponent.getUser() != nullptr && opponent.getUser()->getSocket() != -1 ) {
	  send(opponent.getUser()->getSocket(), "**", 2, 0);
	}
      }
    }
  }

  std::cout << "\033[8;20f";
  sendToAllPlayers("\033[8;20f");
  std::vector<Card> cardsOnTheTable = getCardsOnBoard();
  for_each( begin(cardsOnTheTable), end(cardsOnTheTable), printCard );
  std::string board;
  for_each( begin(cardsOnTheTable), end(cardsOnTheTable), [&board](Card &c){ board += cardToString(c);} );
  sendToAllPlayers(board);

  std::cout << "\033[7;18fPot: " << potSize;
  sendToAllPlayers("\033[7;18fPot: " + std::to_string(potSize));

  std::cout << "\033[18;0f";
  sendToAllPlayers("\033[18;0f");
  
}

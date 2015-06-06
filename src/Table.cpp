#include "../include/Table.h"
#include <ctime>
#include <iostream>
#include <algorithm>

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


Table::Table():players{0},board{0},tableId{nextTableId++},potSize{0}{}
unsigned int Table::nextTableId = 0;

int Table::getTableId() { return tableId; }
unsigned int Table::setPotSize( unsigned int value ) { return potSize = value;}
unsigned int Table::getPotSize() { return potSize; }
std::vector<Player> Table::getPlayers() { return players; }
void Table::addCardToBoard( Card c ) { board.push_back(c); }
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

Player* Table::getPlayerAfter( Player *p )
{
  if( p == nullptr ) {
    return nullptr;
  }

  unsigned int tablePosition = p->getTablePosition();
  Player* after = &players[0];
  for( auto &p : players ) {
    if( p.getTablePosition() > tablePosition ) after = &p;
  }
  return after;
}

Player* Table::findSmallBlind( unsigned int dealerPosition )
{
  return getPlayerAfter( getPlayerAtPosition(dealerPosition) );
}
Player* Table::findBigBlind( unsigned int dealerPosition )
{
  return getPlayerAfter( getPlayerAfter( getPlayerAtPosition(dealerPosition) ) );
}
Player* Table::getPlayerAtPosition( unsigned int position )
{
  for( auto &p: players ) {
    if( p.getTablePosition() == position) return &p;
  }
  return nullptr;
}
//  Player( User *user, unsigned int stackSize, unsigned int tablePosition );

/*class Player { //One instance of a user at a table.
  public:
  Player( User *user, unsigned int stackSize, unsigned int tablePosition );
  int getStackSize();
  int reduceStackSize( int amount );
  int increaseStackSize( int amount );

  Action promptForAction( Action &actionToMatch );

  void setFirstCard( Card c );
  void setSecondCard( Card c );
  Card* getFirstCard();
  Card* getSecondCard();

  private:
  int stackSize;
  User *user; //if nullptr, the player is a computer.
  Card cards[2];
  unsigned int tablePosition;
  //Add user
  };
*/
Player::Player():stackSize(0),user(nullptr),tablePosition(-1) {}
Player::Player(User *user, unsigned int stackSize, unsigned int tablePosition ):stackSize{stackSize},user{user},tablePosition{tablePosition}{}
unsigned int Player::getStackSize() { return stackSize; }
unsigned int Player::reduceStackSize( int amount ) { return stackSize -= amount; }
unsigned int Player::increaseStackSize( int amount ) { return stackSize += amount; }
unsigned int Player::getTablePosition() { return tablePosition; }

Action Player::promptForAction( Action &actionToMatch )
{
  Action playerAction{ActionType::FOLD,0};

  if( user == nullptr ) { //Computer should decide

    playerAction.action = ActionType::BET;
    playerAction.amount = 100;

  } else if( user->getSocket() == -1 ) { //local user

    char action = 0;
    while( action == 0 ) {
      std::cout << "What action do you want to take?(b: bet, f:fold, c: call, r: raise, s: check) :";
      std::cin >> action;

      switch( action ) {
      case 'b':
        playerAction.action = ActionType::BET;
	std::cout << "How much?:";
	std::cin >> playerAction.amount;
	break;
      case 'f':
        playerAction.action = ActionType::FOLD;
	break;
      case 'c':
        playerAction.action = ActionType::CALL;
	break;
      case 'r':
        playerAction.action = ActionType::RAISE;
	std::cout << "How much?:";
	std::cin >> playerAction.amount;
	break;
      case 's':
        playerAction.action = ActionType::CHECK;
	break;
      default:
        std::cout << "Invalid action! try again..\n";
	action = 0;
      }
    }
    
  } else { //Remote party send request.

  }
  return playerAction;
}

/*enum ActionType { FOLD, BET, CALL, RAISE, CHECK };
  struct Action {
  ActionType action;
  int amount;
  };*/

void Player::setFirstCard( Card c ) { cards[0] = c; }
void Player::setSecondCard( Card c ) { cards[1] = c; }
Card Player::getFirstCard() { return cards[0]; }
Card Player::getSecondCard() { return cards[1]; }

/*class User {
  static int nextUserId;

  public:
  User( unsigned int bankroll );
  User( int socket, usigned int bankroll );

  private:
  unsigned int bankroll;
  int socket; //in case of connected human.
  };*/

User::User( unsigned int bankroll ): bankroll{bankroll},socket{-1} {}
User::User( unsigned int bankroll, int socket ): bankroll{bankroll},socket{socket}  {}
int User::getSocket() { return socket; }
int User::nextUserId = 0;

#include "../include/Table.h"
#include <ctime>

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

return true;
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
Action promptForAction( Action &actionToMatch )
{
Action a;
return a;
}

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
int User::nextUserId = 0;

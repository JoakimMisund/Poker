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
std::vector<Player> &Table::getPlayers() { return players; }
void Table::addCardToBoard( Card c ) { board.push_back(c); }
std::vector<Card> Table::getCardsOnBoard() { return board; }
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

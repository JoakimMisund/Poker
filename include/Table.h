#ifndef TABLE_H
#define TABLE_H

#include "CardDeck.h"
#include "User.h"
#include "Player.h"
#define MAX_NR_PLAYERS 8


class Table {
  static unsigned int nextTableId;
public:
  Table();
  int getTableId();
  unsigned int setPotSize( unsigned int value);

  std::vector<Player*> getPlayers();
  void addCardToBoard( Card c );
  unsigned int getNumCards();
  std::vector<Card> getCardsOnBoard();
  bool registerUser( User *user, unsigned int tablePosition );
  Player* getPlayerAfter( Player *p );
  Player* findSmallBlind( unsigned int dealerPosition );
  Player* findBigBlind( unsigned int dealerPosition );
  Player* getPlayerAtPosition( unsigned int position );
private:
  std::vector<Player> players;
  std::vector<Card> board;

  unsigned int tableId;
};

#endif

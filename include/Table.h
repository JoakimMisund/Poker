#ifndef TABLE_H
#define TABLE_H

#include "CardDeck.h"
#include "User.h"
#include "Player.h"
#include "Utils.h"

#define MAX_NR_PLAYERS 8


class Table {
  static unsigned int nextTableId;
public:
  Table();
  int getTableId();

  std::vector<Player*> getPlayers();
  std::vector<Card>& getCardsOnBoard();
  Player* getPlayerAfter( Player *p, std::vector<Player*> &players );
  Player* getPlayerAtPosition( unsigned int position, std::vector<Player*> &players );
  Player* getPlayerAtPosition( unsigned int position );

  void addCardToBoard( Card c );
  unsigned int getNumCards();

  bool registerUser( User *user, unsigned int tablePosition );

  Player* findSmallBlind( unsigned int dealerPosition, std::vector<Player*> &players );
  Player* findBigBlind( unsigned int dealerPosition, std::vector<Player*> &players );

  std::vector<Player*> findWinners( std::vector<Player*> &players );
  void giveTwoCardsToPlayers( std::vector<Player*> players );

  void draw( unsigned int potSize );

  void runTable();
  void runHand( std::vector<Player*> playersInHand );

private:
  std::vector<Player> players;
  std::vector<Card> board;

  unsigned int tableId;
  unsigned int smallBlind;
  unsigned int bigBlind;
  CardDeck deck;
};

#endif

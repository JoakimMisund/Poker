#ifndef TABLE_H
#define TABLE_H

#include "CardDeck.h"

#define MAX_NR_PLAYERS 8

class Player;
class User;

enum ActionType { FOLD, BET, CALL, RAISE, CHECK };
struct Action {
  ActionType action;
  int amount;
};

class Table {
  static unsigned int nextTableId;
public:
  Table();
  int getTableId();
  unsigned int setPotSize( unsigned int value);
  unsigned int getPotSize();
  std::vector<Player> getPlayers();
  void addCardToBoard( Card c );
  unsigned int getNumCards();
  bool registerUser( User *user, unsigned int tablePosition );

private:
  std::vector<Player> players;
  std::vector<Card> board;

  unsigned int tableId;
  unsigned int potSize;
};

class User {
  static int nextUserId;

public:
  User( unsigned int bankroll );
  User( unsigned int bankroll, int socket );
  int getSocket();
  
private:
  unsigned int bankroll;
  int socket; //in case of connected human.
};


class Player { //One instance of a user at a table.
 public:
  Player();
  Player( User *user, unsigned int stackSize, unsigned int tablePosition );
  unsigned int getStackSize();
  unsigned int reduceStackSize( int amount );
  unsigned int increaseStackSize( int amount );
  unsigned int getTablePosition();

  Action promptForAction( Action &actionToMatch );

  void setFirstCard( Card c );
  void setSecondCard( Card c );
  Card getFirstCard();
  Card getSecondCard();

private:
  unsigned int stackSize;
  User *user; //if nullptr, the player is a computer.
  Card cards[2];
  unsigned int tablePosition;
  //Add user
};


#endif

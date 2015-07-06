#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include "../include/CardDeck.h"
#include "../include/User.h"

enum ActionType { FOLD, BET, CALL, RAISE, CHECK };
struct Action {
  ActionType action;
  int amount;
};

enum PlayerState { ACTIVE, AWAY };


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

  PlayerState getState();
  void setPlayerState( PlayerState state );

  bool folded;

 private:
  unsigned int stackSize;
  User *user; //if nullptr, the player is a computer.
  Card cards[2];
  unsigned int tablePosition;
  PlayerState state;
  //Add user
};


#endif

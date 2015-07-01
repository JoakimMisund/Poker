#ifndef PLAYER_H
#define PLAYER_H

enum ActionType { FOLD, BET, CALL, RAISE, CHECK };
struct Action {
  ActionType action;
  int amount;
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

  bool folded;

 private:
  unsigned int stackSize;
  User *user; //if nullptr, the player is a computer.
  Card cards[2];
  unsigned int tablePosition;
  //Add user
};


#endif

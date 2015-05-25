#include "../include/CardDeck.h"
#include <iostream>
#include <ctime>

class Table;

int main( int argc, char *argv[] )
{
  CardDeck deck(time(NULL));

  
  std::cout << cardToString( deck.getNextCard() ) << "\n";

  Table t1 = startNewTable();
  
  return 0;
}

enum ActionType { FOLD, BET, CALL, RAISE, CHECK };
struct Action {
  
int amount;
};

class User {
  static int nextUserId;

public: 
  User();
  
private:
  int socket; //in case of connected human.
};
int User::nextUserId = 0;

class Player { //One instance of a user at a table.
public:
  Player();
  int getStackSize();
  int reduceStackSize( int amount );
  int increaseStackSize( int amount );
  Action promptForAction( Action &actionToMatch );
private:
  int stackSize;
  User *user; //if nullptr, the player is a computer.
  Table *table;
  int tablePosition;
  //Add user
};

class Table {

  static int nextTableId;

public:
  Table();
  int getTableId();
  std::vector<Player> getPlayers();
  bool registerUser( int tablePosition );
private:
  std::vector<Player> players;
  int tableId;
  int dealerIndex;
  int potsize;
  std::vector<Card> board;

};

int Table::nextTableId = 0;

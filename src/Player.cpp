#include "../include/Player.h"
#include "../include/Utils.h"
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <stack>
#include <chrono>



Player::Player():stackSize(0),user(nullptr),tablePosition(-1),state{ACTIVE} {}
Player::Player(User *u, unsigned int stackSize, unsigned int tablePosition ):stackSize{stackSize},user{u},tablePosition{tablePosition},state{ACTIVE}
{
  user = u;
  if( user != nullptr ) std::cout << "Scoekt: " << user->getSocket() << "\n";
}
unsigned int Player::getStackSize() { return stackSize; }
unsigned int Player::reduceStackSize( int amount ) { return stackSize -= amount; }
unsigned int Player::increaseStackSize( int amount ) { return stackSize += amount; }
unsigned int Player::getTablePosition() { return tablePosition; }


void getInput(int socket, char* buffer)
{
  
  recv(socket, buffer, 1000, 0);

}

int getInt( int socket, int &buffer )
{
  std::cout << "getint\n";
  char buf[50] = { 0 };
  getInput( socket, buf );

  std::string s(buf);
  std::size_t t;
  buffer = std::stoi(s, &t);
  return buffer;
}

Action Player::promptForAction( Action &actionToMatch, unsigned int outstandingBet )
{
  Action playerAction{ActionType::FOLD,0};
  ActionType toMatch = actionToMatch.action;
  unsigned int amount = actionToMatch.amount;

  Card c1 = getFirstCard();
  Card c2 = getSecondCard();

  if( user == nullptr ) { //Computer should decide

    if( toMatch == ActionType::FOLD || toMatch == ActionType::CHECK ) {
    
      playerAction.action = ActionType::BET;

      if( static_cast<int>(c2.type)+static_cast<int>(c1.type) >= 16) {
	  playerAction.amount = 300;
      } else if( static_cast<int>(c2.type)+static_cast<int>(c1.type) >= 10) {
	  playerAction.amount = 150;
      } else {
	  playerAction.amount = 100;
      }
    } else if( getStackSize()+outstandingBet >= amount) {
 
      if( amount > 500 ) {
	playerAction.action = ActionType::FOLD;
      } else {
	playerAction.action = ActionType::CALL;
	playerAction.amount =  amount;
      }
    } else {
      playerAction.action = ActionType::CALL;
      playerAction.amount =  std::min( getStackSize(), amount );
    }
    
  } else { //local user
    
    int sock = user->getSocket();

    std::string action_str;
    char action = 0;
    while( action == 0 ) {
      if( static_cast<unsigned int>(actionToMatch.amount) >= getStackSize()+outstandingBet ) {
	action_str = "What action do you want to take?(c: call, f:fold) :";
      } else if( toMatch == ActionType::FOLD ) {
	action_str = "What action do you want to take?(b: bet, f:fold, s: check) :";
      } else if( toMatch == ActionType::BET ) {
	action_str = "What action do you want to take?(f:fold, c: call, r: raise) :";
      } else if( toMatch == ActionType::RAISE ) {
	action_str = "What action do you want to take?(f:fold, c: call, r: raise) :";
      } else if( toMatch == ActionType::CHECK ) {
	action_str = "What action do you want to take?(b: bet, f:fold, s: check) :";
      }

      displayString(action_str, sock);

      getInput( sock, &action );
      fprintf(stderr, "Received: %d %c", action, action);
      //char flush[100];
      //getInput( sock, flush );

      switch( action ) {
      case 'b':
        playerAction.action = ActionType::BET;
	displayString( "How much?:", sock );
	getInt(sock, playerAction.amount);
	if( playerAction.amount > getStackSize()+outstandingBet ) { displayString( "You dont have that much!\n", sock ); action = 0; };
	break;
      case 'f':
        playerAction.action = ActionType::FOLD;
	break;
      case 'c':
        playerAction.action = ActionType::CALL;
	playerAction.amount = (getStackSize()+outstandingBet >= static_cast<unsigned int>(actionToMatch.amount)) ?
	  actionToMatch.amount:getStackSize()+outstandingBet;
	break;
      case 'r':
        playerAction.action = ActionType::RAISE;
	displayString( "How much?:", sock );
	getInt(sock, playerAction.amount);
	if( playerAction.amount > getStackSize()+outstandingBet ) { displayString( "You dont have that much!\n", sock ); action = 0; };
        if( playerAction.amount <= actionToMatch.amount ) { displayString("You have to raise to a amount higher than the current bet!\n", sock); action = 0;};
	break;
      case 's':
        playerAction.action = ActionType::CHECK;
	break;
      default:
        displayString( "Invalid action! try again..\n", sock );
	action = 0;
      }
    }
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

PlayerState Player::getState() { return state; }

void Player::drawCards()
{


   if( user == nullptr ) {
     //     printCard(getFirstCard());
     //printCard(getSecondCard());
   } else {
     int sock = user->getSocket();
     std::string cards = cardToString(getFirstCard()) + cardToString(getSecondCard());
     send(sock, cards.c_str(), cards.length(), 0);
   }
}

#include "../include/Player.h"
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include "../include/Utils.h"


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

Action Player::promptForAction( Action &actionToMatch )
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
    } else if( getStackSize() >= amount) {
 
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
    
  } else if( user->getSocket() == -1 ) { //local user

    char action = 0;
    while( action == 0 ) {
      if( static_cast<unsigned int>(actionToMatch.amount) >= getStackSize() ) {
	std::cout << "What action do you want to take?(c: call, f:fold) :";
      } else if( toMatch == ActionType::FOLD ) {
	std::cout << "What action do you want to take?(b: bet, f:fold, s: check) :";
      } else if( toMatch == ActionType::BET ) {
	std::cout << "What action do you want to take?(f:fold, c: call, r: raise) :";
      } else if( toMatch == ActionType::RAISE ) {
	std::cout << "What action do you want to take?(f:fold, c: call, r: raise) :";
      } else if( toMatch == ActionType::CHECK ) {
	std::cout << "What action do you want to take?(b: bet, f:fold, s: check) :";
      }


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
	playerAction.amount = (getStackSize() >= static_cast<unsigned int>(actionToMatch.amount)) ? actionToMatch.amount:getStackSize();
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

    int sock = user->getSocket();
    char buf[100] = "Choose action:";
    send( sock, buf, 14, 0);
    
    char resp[100] = {0};
    recv(sock, resp, 100, 0);
    
    playerAction.action = ActionType::CALL;
    
    std::cout << resp;
    
    exit(-1);

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
     printCard(getFirstCard());
     printCard(getSecondCard());
   } else {
     int sock = user->getSocket();
     std::string cards = cardToString(getFirstCard()) + cardToString(getSecondCard());
     send(sock, cards.c_str(), cards.length(), 0);
   }
}

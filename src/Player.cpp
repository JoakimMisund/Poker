#include "../include/Player.h"

Player::Player():stackSize(0),user(nullptr),tablePosition(-1),state{ACTIVE} {}
Player::Player(User *user, unsigned int stackSize, unsigned int tablePosition ):stackSize{stackSize},user{user},tablePosition{tablePosition},state{ACTIVE}{}
unsigned int Player::getStackSize() { return stackSize; }
unsigned int Player::reduceStackSize( int amount ) { return stackSize -= amount; }
unsigned int Player::increaseStackSize( int amount ) { return stackSize += amount; }
unsigned int Player::getTablePosition() { return tablePosition; }

Action Player::promptForAction( Action &actionToMatch )
{
  Action playerAction{ActionType::FOLD,0};
  ActionType toMatch = actionToMatch.action;
  int amount = actionToMatch.amount;

  if( user == nullptr ) { //Computer should decide

    if( toMatch == ActionType::FOLD || toMatch == ActionType::CHECK ) {
    
      playerAction.action = ActionType::BET;
      playerAction.amount = 100;
    } else {
      if( amount > 500 ) {
	playerAction.action = ActionType::FOLD;
      } else {
	playerAction.action = ActionType::CALL;
	playerAction.amount = amount;
      }
    }
    
  } else if( user->getSocket() == -1 ) { //local user

    char action = 0;
    while( action == 0 ) {
      if( toMatch == ActionType::FOLD ) {
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

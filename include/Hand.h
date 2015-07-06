#ifndef HAND_H
#define HAND_H

#include "Player.h"
#include "Table.h"

class Hand {

 private:
  std::vector<Player*> playersInHand;
  unsigned int potSize;
  
 public:

  Hand( std::vector<Player*> &players );

  unsigned int getPotSize();
  unsigned int setPotSize();

  
};

#endif

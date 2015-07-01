#ifndef HAND_H
#define HAND_H

#include "Player.h"
#include "Table.h"

class Hand {

 private:
  std::vector<Player*> playersInHand;
  unsigned int potSize;
  Table *table;
  
 public:
  unsigned int getPotSize();
  
};

#endif

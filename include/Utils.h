#ifndef UTILS_H
#define UTILS_H

#include "CardDeck.h"
#include "Table.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <cassert>
#include "Player.h"

enum handStrength{ HIGH_CARD = 0, PAIR = 1, TWO_PAIRS = 2, THREE_OF_A_KIND = 3, STRAIGHT = 4, FLUSH = 5, FULL_HOUSE = 6, FOUR_OF_A_KIND = 7, STRAIGHT_FLUSH = 8 };

std::vector<Card> findBestCombination( std::vector<Card> &cards );
void print_hand( std::vector<Card> &hand );
handStrength getHandStrength( std::vector<Card> hand, Type &t1, Type &t2 );


int compareHands( std::vector<Card> hand1, std::vector<Card> hand2 );

void displayString( std::string s, int sock );

#endif

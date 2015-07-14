#include "../include/Utils.h"

int compareHands( std::vector<Card> hand1, std::vector<Card> hand2 )
{
  Type h1t1, h2t1;
  Type h1t2, h2t2;
  handStrength h1 = getHandStrength( hand1, h1t1, h1t2 );
  handStrength h2 = getHandStrength( hand2, h2t1, h2t2 );

  if(h1 == h2 && h1t1 == h2t1 && h1t2 == h2t2 ) return 0;

  if( h1 != h2 ) {
    return (h1 > h2) ? 1:-1;
  }
  

  if( h1t1 != h2t1 ) {
    return (h1t1 > h2t1) ? 1:-1;
  }
  if( h1 == STRAIGHT || h1 == STRAIGHT_FLUSH ) return 0;

  if( h1t2 != h2t2 ) {
    return (h1t2 > h2t2) ? 1:-1;
  }

  //Highest card desides.
  std::sort( begin(hand1), end(hand1), []( Card c1, Card c2 ) {
      return c1.type < c2.type;
    } );
  std::sort( begin(hand2), end(hand2), []( Card c1, Card c2 ) {
      return c1.type < c2.type;
    } );

  for( int i = hand1.size()-1; i >= 0; --i ) {
    if( hand1[i].type == h1t1 || hand1[i].type == h1t2) continue;
    if( hand1[i].type > hand2[i].type ) return 1;
    else if( hand1[i].type < hand2[i].type ) return -1;
  }

  
  return 0;

}

void print_hand( std::vector<Card> &hand )
{

  std::string types[]{"two","three","four","five","six","seven","eight","nine","ten","jack","queen","king","ace"};
  std::string suits[]{"heart","club","spade","diamond"};
  for( Card &c : hand ) {
    std::cout << types[c.type] << " of " << suits[c.suit] << " ";
  }

  std::cout << "\n";
  for_each( begin(hand), end(hand), 
	    [](const Card &c) { printCard(c); std::cout << " "; } );
  std::cout << "\n";
}

std::vector<Card> findBestCombination( std::vector<Card> &cards )
{
  assert( cards.size() == 7 );
  
  std::vector<Card> currentHand;
  std::vector<Card> bestHand;

  //i is the index of a card to exclude
  for( unsigned int i = 0; i < cards.size(); ++i ) {
    //j is the index of a card to exclude.
    for( unsigned int j = i+1; j < cards.size(); ++j ) {
    
      currentHand.clear();
      //add cards
      for( unsigned int k = 0; k < cards.size() && currentHand.size() != 5; ++k ) {
	if( k != i && k != j ) {
	  currentHand.push_back(cards[k]);
	}
      }

      if( bestHand.size() == 0 ) {
	bestHand = currentHand;
      } else {
	
	/*	std::cout << "Best so far: ";
	for_each( begin(bestHand), end(bestHand), printCard );
	std::cout << " Challenger: ";
	for_each( begin(currentHand), end(currentHand), printCard );
	std::cout << "\n";
	*/
	if( compareHands( currentHand, bestHand ) == 1 ) {
	  bestHand = currentHand;
	}
      }
      
    }
  }

  return bestHand;

}

handStrength getHandStrength( std::vector<Card> hand, Type &t1, Type &t2 )
{
  std::sort( begin(hand), end(hand), []( Card c1, Card c2 ) {
      return c1.type < c2.type;
    } );

  t1 = t2 = Type::TWO; //added to make the callers job easier. Does not have to check what strength

  //check is straight or flush or both
  Type highCard = Type::TWO;
  Type t = hand[0].type;
  Suit s = hand[0].suit;
  bool straight = true;
  bool flush = true;

  for( unsigned int i = 1; i < hand.size(); ++i ) { //check flush and straight
    Card curr = hand[i];
 
    if( curr.type != t+1 ) { //not a straight
      straight = false;
    }
    if( curr.suit != s ) { //not a flush
      flush = false;
    }
    t = curr.type;


    highCard = (highCard < curr.type) ? curr.type:highCard;
  }

  if( straight && flush ) {
    t1 = highCard;
    return STRAIGHT_FLUSH;
  } else if( straight ) {
    t1 = highCard;
    return STRAIGHT;
  } else if( flush ) {
    t1 = highCard;
    return FLUSH;
  }

  //find max of a kind
  unsigned short max = 0;
  t = Type::TWO;
  for( unsigned int i = 0; i < hand.size(); ++i ) { //go through all cards
    Card c = hand[i];
    unsigned short count = 1; //how many do i have of this kind?
    for( unsigned int j = i+1; j < hand.size(); ++j ) {
      if( hand[j].type == c.type ) count++;
    }

    if( count == 4 ) {
      t1 = c.type;
      return FOUR_OF_A_KIND;
    } else if( count == 3 && max == 2 ) {
      t1 = c.type;
      t2 = t;
      return FULL_HOUSE;
    } else if( count == 2 && max == 3 && t != c.type ) {
      t2 = c.type;
      t1 = t;
      return FULL_HOUSE;
    } else if( count == 2 && max == 2 ) {
      t1 = c.type;
      t2 = t;
      return TWO_PAIRS;
    }

    if( count > max || ( count == max && c.type > t ) ) { //new best
      max = count;
      t = c.type;
    }
  }

  t1 = t;

  if( max == 1 ) {
    return HIGH_CARD;
  } else if( max == 2 ) {
    return PAIR;
  } else if( max == 3 ) {
    return THREE_OF_A_KIND;
  }

  std::cout << "Should never be here!!!!\n";
  return HIGH_CARD;
}

#include "../include/CardDeck.h"
#include "../include/Table.h"
#include <iostream>
#include <ctime>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>
#include "../include/Utils.h"
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <map>

using namespace std::chrono;


void test_getHandStrength();
void test_findBestCombination();

int setupServerSocket( char* portnumber )
{
  int sock;
  struct addrinfo hints;
  struct addrinfo *res;
  int optval = 1;

  memset( &hints, 0, sizeof(hints));

  //setup hints
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if( getaddrinfo(NULL, portnumber, &hints, &res ) != 0 ) {
    std::cerr << "Error calling getaddrinfo\n";
    return -1;
  }
  struct addrinfo *addr;

  for( addr = res; addr != NULL; addr = addr->ai_next ) {

    sock = socket( addr->ai_family, addr->ai_socktype, addr->ai_protocol);

    if(sock > 0 ) {
      setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
      if( bind(sock, addr->ai_addr, addr->ai_addrlen) == 0 ) break;
      else close(sock);
    }
  }

  freeaddrinfo(res);

  if( addr == NULL ) {
    std::cerr << "could not bind to this addr\n";
    return -1;
  }

  if( listen(sock,10) != 0 ) {
    std::cerr << "could not listen to the socket\n";
    return -1;
  }

  return sock;
}

void runThread( int conn_sock )
{
  Table t;
  User *u = new User(15000, conn_sock);
  t.registerUser( u, 0 ); //add a User

  int nrOpponents = -1;;
  while( ! (nrOpponents >= 1 && nrOpponents <= 7) ) {
    send(conn_sock, "How many opponents do you want? (Minumim 1, maximum 7):", 56, 0);

    char resp[2];
    if( recv(conn_sock, &resp, sizeof(resp), 0) <= 0 ) {
      std::cerr << "Client has exited\n";
      close(conn_sock);
      return;
    }
  
    nrOpponents = resp[0]-'0';
  }

  

  for( int i = 0; i < nrOpponents; ++i ) {
    t.registerUser( nullptr, i+1 );
  }

  t.runTable();
}



int main( int argc, char *argv[] )
{
#ifdef DEBUG
  test_findBestCombination();
  test_getHandStrength();
#endif

  //construct tables

  if( argc < 2 ) {
    std::cerr << "Two few arguments. USAGE: " << argv[0] << " [Portnumber]\n";
    return -1;
  }

  int socket = setupServerSocket( argv[1] );
  if( socket < 0 ) {
    std::cerr << "Socket could not be set up\n";
    return -1;
  }

  std::cout << "socket: " << socket << "\n";

  struct epoll_event ev, events[10];
  int nfds, epollfd;

  if( (epollfd = epoll_create(10)) == -1 ) {
    std::cerr << "Could not create epoll\n";
    return -1;
  }

  ev.events = EPOLLIN;
  ev.data.fd = socket;
  if( epoll_ctl(epollfd, EPOLL_CTL_ADD, socket, &ev ) == -1 ) {
    std::cerr << "epoll_ctl: socket\n";
    return -1;
  }

  std::atomic<bool> done{false};
  std::thread *f = nullptr;
  std::map<int, std::thread*> thdrs;

  while(1) {
    
    nfds = epoll_wait( epollfd, events, 10, duration_cast<milliseconds>(seconds(2)).count() );
    if( nfds == -1 ) {
      std::cerr << "epoll_wait\n";
      return -1;
    }

    for( int i = 0; i < nfds; ++i ) {
      if( events[i].data.fd == socket ) {
	int conn_sock = accept( socket, NULL, NULL );
	if( conn_sock == -1 ) {
	  std::cerr << "Socket could not be set up, accept\n";
	  return -1;
	}

	/*	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = conn_sock;
	if( epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1 ) {
	  std::cerr << "epoll_ctl\n";
	  return -1;
	  }*/


	std::cout << conn_sock << "\n";
	new std::thread([conn_sock] {
	    runThread(conn_sock);
	  });
	

      } else {
	char buf[100]{0};
	if( read( events[i].data.fd, buf, 100 ) == 0 ) {
	  epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
	  close( events[i].data.fd );
	  std::cout << "Client has exited\n";
	}
	std::cout << buf;
	//TODO close and remove when exit
      }
    }
  }
  
  close(epollfd);
  close(socket);

  return 0;
  /*
  Table t;
  User user(15000);
  t.registerUser( &user, 0 ); //add a User
  t.registerUser( nullptr, 1 ); //add a computer
  t.registerUser( nullptr, 2 ); //add a computer
  t.registerUser( nullptr, 3 ); //add a computer
  t.registerUser( nullptr, 4 ); //add a computer
  t.registerUser( nullptr, 5 ); //add a computer
  t.registerUser( nullptr, 6 ); //add a computer
  t.registerUser( nullptr, 7 ); //add a computer
  t.runTable();*/
  return 0;
}

void test_getHandStrength()
{
  CardDeck deck(time(NULL));
  std::string strengths[]{"high card","pair","two pairs", "three of a kind", "straight", "flush", "full house", "four of a kind", "straight flush" };

  //int compareHands( std::vector<Card> hand1, std::vector<Card> hand2 )
  Card c1{DIAMOND, ACE};
  Card c2{HEART, ACE};
  Card c3{CLUB, QUEEN};
  Card c4{SPADE, QUEEN};
  Card c5{DIAMOND, EIGHT};
  std::vector<Card> h1{c1,c2,c3,c4,c5};
  c5.type = TEN;
  std::vector<Card> h2{c1,c2,c3,c4,c5};

  std::cout << "Test: " << compareHands( h1, h2 ) << "\n";
  return;
  
    //a a q q 8 vs a a q q 10
    //7 7 k kn 5 vs 7 7 6 5 8

  //handStrength getHandStrength( std::vector<Card> &hand, Type &t1, Type &t2 )
  std::vector<Card> hand;
  
  for( int i = 0; i < 10; ++i ) { //run test 10 times
    hand.clear();
    deck.resetDeck();
    for( int j = 0; j < 5; ++j ) { //get 5 random cards
      Card c = deck.getNextCard();
      hand.push_back(c);
    }

    print_hand(hand);
    Type t1,t2;
    handStrength strength = getHandStrength( hand, t1, t2);

    std::cout << strengths[strength] << "\n";
    if( strength == handStrength::STRAIGHT_FLUSH ) break;

  }
}

void test_findBestCombination()
{
  //std::vector<Card> findBestCombination( std::vector<Card> cards )
  //input 7 cards, output: 5 cards (the ideal combination)

  CardDeck deck(time(NULL));
  std::vector<Card> possibleCards;
  

 

  for( int i = 0; i < 7; ++i ) { //add 7 cards
    possibleCards.push_back( deck.getNextCard() );
  }

  std::cout << "Cards: ";
  for_each( begin(possibleCards), end(possibleCards), printCard );
 
  std::vector<Card> bestHand = findBestCombination( possibleCards );
  
  std::cout << "\nBest hand: ";
  for_each( begin(bestHand), end(bestHand), printCard );
  std::cout << "\n";
}

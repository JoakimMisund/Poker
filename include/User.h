#ifndef USER_H
#define USER_H

class User {
  static int nextUserId;

public:
  User( unsigned int bankroll );
  User( unsigned int bankroll, int socket );
  int getSocket();
  
private:
  unsigned int bankroll;
  int socket; //in case of connected human.
};

#endif

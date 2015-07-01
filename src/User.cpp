#include "../include/User.h"

User::User( unsigned int bankroll ): bankroll{bankroll},socket{-1} {}
User::User( unsigned int bankroll, int socket ): bankroll{bankroll},socket{socket}  {}
int User::getSocket() { return socket; }
int User::nextUserId = 0;

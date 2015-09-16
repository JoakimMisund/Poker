CC = g++
CFLAGS = -g -c -Wall -std=c++1y -pedantic
LDFLAGS = -lm -std=c++1y -pedantic -pthread

SRCDIR = src

ODIR = bin
LDIR = lib
IDIR = include

#TODO: Do this automatically
DEPS = include/CardDeck.h include/Table.h include/User.h include/Player.h include/Hand.h
OBJ = bin/CardDeck.o bin/Table.o bin/main.o bin/User.o bin/Player.o bin/Hand.o bin/Utils.o

SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))

$(ODIR)/%.o : src/%.cpp $(DEPS)
	$(CC) $(CFLAGS) -o $@ $<

bin/runner: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)


.PHONY: clean

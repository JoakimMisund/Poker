CC = g++
CFLAGS = -g -c -Wall -std=c++1y -pedantic
LDFLAGS = -lm -std=c++1y -pedantic

SRCDIR = src
BUILDDIR = build
TARGET = bin/runner

SRCEXT = cpp
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS = $(pathsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

LIB =
INC = -I include

$(TARGET) : $(OBJECTS)/%.o
	$(CC) $(LDFLAGS) $(SOURCES) -o bin/runner

$(OBJECTS)/%.o: $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES)

.PHONY: clean

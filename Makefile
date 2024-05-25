CC=clang++
CFLAGS=-Wall -Wextra -std=c++11
LIBS=-lncurses

SRC=main.cpp map.cpp save.cpp

all: constructor

constructor: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o constructor $(LIBS)

clean:
	rm -f constructor

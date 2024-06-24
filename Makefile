CC=clang++
CFLAGS=-Wall -Wextra -std=c++11
LIBS=-lncurses -lmenu

SRC=main.cpp tui/menu.cpp map.cpp save.cpp activity_main_menu.cpp activity_settings.cpp activity_game.cpp

all: constructor

constructor: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o constructor $(LIBS)

clean:
	rm -f constructor

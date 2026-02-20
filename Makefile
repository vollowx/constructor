CC=gcc
CFLAGS=-Wall -Wextra
LIBS=-lncurses -lmenu

SRC=main.c log.c options.c activity_main_menu.c activity_gameplay.c activity_options.c

all: build

build: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o constructor $(LIBS)

clean:
	rm -f constructor

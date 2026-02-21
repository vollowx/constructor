CC=gcc
CFLAGS=-Wall -Wextra
LIBS=-lncurses -lmenu

SRC=main.c game.c log.c options.c save.c am_log.c pm_main_menu.c pm_saves.c pm_gameplay.c pm_options.c

all: build

build: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o constructor $(LIBS)

clean:
	rm -f constructor

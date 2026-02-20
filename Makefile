CC=gcc
CFLAGS=-Wall -Wextra
LIBS=-lncurses -lmenu

SRC=main.c log.c options.c model_log.c model_main_menu.c model_gameplay.c model_options.c

all: build

build: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o constructor $(LIBS)

clean:
	rm -f constructor

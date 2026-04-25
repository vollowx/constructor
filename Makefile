CC=gcc
CFLAGS=-Wall -Wextra -Ithirdparty
LIBS=-lncurses -lmenu

TARGET=constructor

SRC=main.c fcp.c game.c log.c models.c options.c save.c \
    minor_log.c \
    major_main_menu.c major_saves.c major_gameplay.c major_options.c \
    thirdparty/simplexnoise1234.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: clean

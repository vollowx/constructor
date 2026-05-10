CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter -Ithirdparty
LDFLAGS=
LDLIBS=-lncurses -lmenu

TARGET=constructor

SRC=main.c fcp.c game.c log.c app_state.c options.c save.c \
    overlay_log.c \
    screen_main_menu.c \
    screen_saves.c \
    screen_gameplay.c \
    screen_options.c \
    screen_about.c \
    thirdparty/simplexnoise1234.c
OBJ=$(SRC:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJ) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET)

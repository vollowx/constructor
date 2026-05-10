CC=gcc
CFLAGS=-Wall -Wextra -Wno-unused-parameter -Ithirdparty
LDFLAGS=
LDLIBS=-lncurses -lmenu

TARGET=constructor

SRC=main.c fcp.c game.c log.c info.c options.c save.c \
    minor_log.c \
    major_main_menu.c \
    major_saves.c \
    major_gameplay.c \
    major_options.c \
    major_about.c \
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

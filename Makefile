CC=gcc
CFLAGS=-Wall -Wextra -Ithirdparty
LIBS=-lncurses -lmenu

TARGET=constructor

SRC=main.c fcp.c game.c log.c models.c options.c save.c \
    am_log.c \
    pm_main_menu.c pm_saves.c pm_gameplay.c pm_options.c \
    thirdparty/simplexnoise1234.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

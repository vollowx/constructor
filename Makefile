CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -Wno-unused-parameter -Isrc -Ithirdparty
LDFLAGS=-lncurses -lmenu

TARGET=build/constructor

SRC=$(shell find src -name '*.c') thirdparty/simplexnoise1234.c
OBJ=$(patsubst %.c,build/%.o,$(SRC))

.PHONY: all clean compdb

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build

compdb:
	@mkdir -p build
	bear --output build/compile_commands.json -- make all

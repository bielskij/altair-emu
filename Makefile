CC=gcc

DIR_OUT := out
DIR_SRC := src

CFLAGS := -Wall -O2
CFLAGS += -ggdb
CFLAGS += -Ilib/common/inc -Ilib/emulator/inc

all: init
	$(CC) $(CFLAGS) -o $(DIR_OUT)/emulator.elf \
		lib/emulator/src/cpu.c \
		\
		lib/common/src/common/utils/ihex.c \
		\
		main.c
		
init:
	mkdir -p $(DIR_OUT)
	
clean:
	rm -rf $(DIR_OUT)
CC=gcc

ARCH ?= x86

DIR_OUT := out
DIR_SRC := src

CFLAGS := -Wall -O2
CFLAGS += -ggdb
CFLAGS += -Ilib/common/inc -Ilib/emulator/inc -Iarch/inc -Iarch/$(ARCH)/inc

# Altair configuration

# Altair 8800b
CFLAGS += -DALTAIR_MODULES_COUNT=8

LDFLAGS += -lutil

all: init
	$(CC) $(CFLAGS) -o $(DIR_OUT)/emulator.elf \
		lib/emulator/src/cpu.c \
		lib/emulator/src/altair/mainBoard.c \
		lib/emulator/src/altair/module/cpu.c \
		lib/emulator/src/altair/module/88-16mcs.c \
		lib/emulator/src/altair/module/88-2sio.c \
		\
		lib/common/src/common/utils/ihex.c \
		\
		arch/$(ARCH)/src/arch.c \
		arch/$(ARCH)/src/module/88-2sio.c \
		arch/$(ARCH)/src/module/88-16mcs.c \
		\
		main.c \
		$(LDFLAGS)
		
init:
	mkdir -p $(DIR_OUT)
	
clean:
	rm -rf $(DIR_OUT)
CC=gcc

ARCH ?= x86

DIR_OUT := out
DIR_SRC := src

CFLAGS := -Wall -O2
CFLAGS += -ggdb
CFLAGS += -Ilib/common/inc -Ilib/emulator/inc -Iarch/inc -Iarch/$(ARCH)/inc

# Altair configuration

# Altair 8800b
CFLAGS += -DALTAIR_MODULES_COUNT=16
CFLAGS += -DALTAIR_F_CPU=2000000ULL
CFLAGS += -DTICKS_PER_LOOP=80000ULL

LDFLAGS += -lutil

all: init
	$(CC) $(CFLAGS) -o $(DIR_OUT)/emulator.elf \
		lib/emulator/src/cpu.c \
		lib/emulator/src/altair/mainBoard.c \
		lib/emulator/src/altair/module/cpu.c \
		lib/emulator/src/altair/module/88-16mcs.c \
		lib/emulator/src/altair/module/88-2sio.c \
		lib/emulator/src/altair/module/88-sio.c \
		lib/emulator/src/altair/module/88-pmc.c \
		lib/emulator/src/altair/module/88-mcs.c \
		\
		lib/common/src/common/utils/ihex.c \
		\
		arch/$(ARCH)/src/arch.c \
		arch/$(ARCH)/src/serial.c \
		arch/$(ARCH)/src/module/88-sio.c \
		arch/$(ARCH)/src/module/88-2sio.c \
		arch/$(ARCH)/src/module/88-16mcs.c \
		arch/$(ARCH)/src/module/88-pmc.c \
		arch/$(ARCH)/src/module/88-mcs.c \
		\
		main.c \
		$(LDFLAGS)
		
init:
	mkdir -p $(DIR_OUT)
	
clean:
	rm -rf $(DIR_OUT)
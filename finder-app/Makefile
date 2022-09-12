SRC := writer.c
TARGET = writer
OBJS := $(SRC:.c=.o)

# CFLAGS=-g -Wall
LDFLAGS= 

all: $(TARGET)

clean:
	-rm -f *.o $(TARGET) *.elf *.map

ifeq ($(CROSS_COMPILE),aarch64-none-linux-gnu-)
USER_CC=aarch64-none-linux-gnu-gcc
else
USER_CC=gcc
endif

$(TARGET) : $(OBJS)
	$(USER_CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)
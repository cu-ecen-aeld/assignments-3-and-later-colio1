CC = $(CROSS_COMPILE)gcc

CFLAGS  = -pthread -lrt -Wall -Werror

TARGET = aesdsocket

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET) $(TARGET).o

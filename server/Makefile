CC = $(CROSS_COMPILE)gcc

CFLAGS  = -pthread -Wall -Werror

TARGET = aesdsocket

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET) $(TARGET).o

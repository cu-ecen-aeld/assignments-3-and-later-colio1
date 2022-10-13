INCLUDE_DIRS =
LIB_DIRS =

CC = $(CROSS_COMPILE)gcc

CDEFS=
CFLAGS= -O0 -g -Wall $(INCLUDE_DIRS) $(CDEFS)
LIBS=

TARGET= aesdsocket

HFILES=
CFILES= aesdsocket.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	${TARGET}

$(TARGET):	$(TARGET).o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(TARGET).o $(LIBS)

clean:
	rm -f *.o
	rm -f ${TARGET}

depend:

.c.o:
	$(CC) $(CFLAGS) -c $<

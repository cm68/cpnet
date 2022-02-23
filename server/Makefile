SHELL=/bin/sh

CC = cc
#DEBUG = -g -Wall
DEBUG = -g
CFLAGS = -O3
CFLAGS = $(DEBUG)
LIBS =

SRCS = main.c \
	cpnet11.c \
	cpnet12.c \
	cpmutl.c \
	netio.c \
	sio.c \
	sockio.c \
	inifile.c

OBJS = $(SRCS:.c=.o)

PROG = cpnet

.SUFFIX: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<

${PROG}: ${OBJS}
	$(CC) $(DEBUG) -o $@ ${OBJS} $(LIBS)

install: cpnet
	cp cpnet.sh /usr/local/bin/cpnet
	cp cpnet /usr/local/lib
	mkdir -p /usr/local/etc/cpnet
	cp cpnet.ini /usr/local/etc/cpnet

clean:
	rm -f ${OBJS} $(PROG) *~ core *.bak

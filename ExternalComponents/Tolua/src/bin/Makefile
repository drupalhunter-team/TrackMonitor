# makefile for tolua executable

TOLUA=../..

include $(TOLUA)/config

OBJS=	\
	tolua.o \
	toluabind.o

T= $(TOLUA)/bin/tolua

all: $T

$T: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIB) -ltolua -llua -lm

clean:
	rm -f $(OBJS) $T

klean:
	rm -f $T


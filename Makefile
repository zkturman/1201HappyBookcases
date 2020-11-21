CC=gcc
#CFLAGS=-Wall -Wextra -pedantic -std=C90 -O3
CFLAGS=-Wall -Wextra -Wfloat-equal -pedantic -std=c90 -g3 -fsanitize=undefined -fsanitize=address
#CFLAGS=`sdl2-config --cflags` -O2 -Wall -Wextra -pedantic -Wfloat-equal -std=c90
LDLIBS=-lm
#LDLIBS=`sdl2-config --libs` -lm

hb : hc.c
	$(CC) hb.c -o hb $(CFLAGS) $(LDLIBS)

CC=gcc
CFLAGS=-Wall -Wextra -Wfloat-equal -pedantic -std=c90 -O3
#CFLAGS=-Wall -Wextra -Wfloat-equal -pedantic -std=c90 -g3 -fsanitize=undefined -fsanitize=address
#CFLAGS=-Wall -Wextra -Wfloat-equal -pedantic -std=c90 -g3
LDLIBS=-lm

hb : bookcase.c neillsimplescreen.c neillsimplescreen.h
	$(CC) bookcase.c neillsimplescreen.c -o hb $(CFLAGS) $(LDLIBS)

ext : extension.c neillsimplescreen.c neillsimplescreen.h
	$(CC) extension.c neillsimplescreen.c -o ext $(CFLAGS) $(LDLIBS)


valgrindhb:
	$(CC) extension.c neillsimplescreen.c -o ext $(CFLAGS) $(LDLIBS)
	valgrind --leak-check=full ./hb rggrcyyc-557.bc

valgrindext:
	$(CC) extension.c neillsimplescreen.c -o ext $(CFLAGS) $(LDLIBS)
	valgrind --leak-check=full -s ./ext rggrcyyc-557.bc

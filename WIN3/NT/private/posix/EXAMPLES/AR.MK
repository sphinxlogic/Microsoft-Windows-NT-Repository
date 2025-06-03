INC = //C/nt/public/sdk/inc
CCFLAGS = $(CFLAGS) -I $(INC)/posix -I $(INC)/crt

test.a: a.o b.o
	ar -rcv test.a a.o b.o

a.o: a.c
	cc -c $(CCFLAGS) a.c

b.o: b.c
	cc -c $(CCFLAGS) b.c

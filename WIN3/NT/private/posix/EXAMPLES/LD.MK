SDK = //C/nt/public/sdk
INC = $(SDK)/inc
CCFLAGS = $(CFLAGS) -I $(INC)/posix -I $(INC)/crt
LDFLAGS = -L $(SDK)/lib/i386

test: test.o test.a
	ld -o test $(LDFLAGS) test.o test.a

test.o: test.c
	cc -c $(CCFLAGS) test.c

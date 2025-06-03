SDK = //C/nt/public/sdk

test: test.c a.c b.c
	cc -o test $(CFLAGS) -I $(SDK)/inc/crt -L $(SDK)/lib/i386 *.c

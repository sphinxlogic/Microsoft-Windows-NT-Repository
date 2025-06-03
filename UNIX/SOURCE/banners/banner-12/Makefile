################################################################################
#                             Makefile for "banner"                            #
################################################################################

#CFLAGS = -g
CFLAGS = -O
LFLAGS =

INSTALL_BANNER = /usr/local/bin/Banner

OBJ = main.o banner.o font.o

all: banner

install: banner
	cp banner $(INSTALL_BANNER)
	strip $(INSTALL_BANNER)
	chmod 755 $(INSTALL_BANNER)

clean:
	rm -f $(OBJ) banner

banner: $(OBJ)
	cc $(LFLAGS) $(OBJ) -o banner

lint:
	lint -bchux *.c

main.o: main.c flags.h

banner.o: banner.c flags.h

font.o: font.c

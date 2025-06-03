#
#	$Id: Makefile,v 1.1 1992/12/22 09:06:18 joke Exp $
#

CC	= gcc
CFLAGS	= -I. -O3 -g

SRCS	= ./*.c
HDRS	= ./*.h

PRG	= b2c
VERS	= $(PRG)-1.00j
prefix 	= /opt
mandir	= /opt/man/cat1
mansuffix = 1

DEMO	= you
TROFF	= troff


# all:	getopt $(PRG) $(DEMO)
all:	$(PRG) $(DEMO) ${PRG}.${mansuffix} ${PRG}.ps

getopt:	getopt.o

$(PRG):	$(PRG).o getopt.o
	$(CC) -s -o $(PRG) $(PRG).o getopt.o -L/usr/ucblib -lucb

install:
	cp ${PRG} ${prefix}/bin
	cp ${PRG}.${mansuffix} ${mandir}
	

######## demonstration ######################################################
$(DEMO):	$(DEMO).o
	$(CC) -s -o $(DEMO) $(DEMO).o

$(DEMO).o:	youvegot.h

youvegot.h: $(PRG)
	./$(PRG) -i youvegot -D YOUVESIZ -G _youvegot_h_ -o youvegot.h youvegotmail.au

######## neating #############################################################
dvi:
	tgrind -h $(VERS) $(SRCS) $(HDRS)

######## packaging ###########################################################
${PRG}.${mansuffix}:
	nroff -man $(PRG).1 >$(PRG).${mansuffix}

${PRG}.ps:
	$(TROFF) -man $(PRG).1 >$(PRG).ps

######## packaging ###########################################################
DIST	= Makefile MAGPIE LICENSE README getopt.c getopt1.c getopt.h $(PRG).c $(PRG).1 $(PRG).man $(DEMO).c youvegot.h youvegotmail.au

tar:	youvegot.h man
	$(RM) $(VERS).tar*
	tar cvf $(VERS).tar $(DIST)
	compress $(VERS).tar

uue:	tar $(PRG)
	uuencode $(VERS).tar.Z $(VERS).tar.Z >$(VERS).uue

shar:	youvegot.h man
	shar -c -n "$(VERS)" -a -M \
		-s joke@ls11.informatik.uni-dortmund.de \
		-f -l50 -d "*** MAGPIE ***" -o "$(VERS).shar" \
		$(DIST)


######## homing ##############################################################
disk:	youvegot.h man
	mcopy -t $(TARS) a:

######## cleanup #############################################################
clean:
	$(RM) *~ *.o *.BAK *.man *.ps $(VERS).* $(PRG) $(DEMO)

clobber: clean
	$(RM) $(PRG) $(DEMO) youvegot.h

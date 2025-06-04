# Generated automatically from Makefile.in by configure.
CC     = cc
CFLAGS =   
LIBS   = libaa.a -lcurses   -ldnet  -lX11 -lgpm  -lm
LFLAGS = 
prefix= /usr/local
exec_prefix= ${prefix}
datadir= ${prefix}/share
bindir= ${exec_prefix}/bin
libdir= ${exec_prefix}/lib
includedir= ${prefix}/include
mandir= ${prefix}/man
infodir= ${prefix}/info


INCL = config.h 
 
OBJS = aafont.o \
       aafonts.o \
       aamktabl.o \
       aamem.o \
       aahelp.o \
       aaslnkbd.o \
       aaslang.o \
       aafastre.o \
       aalinux.o \
       aax.o \
       aaxkbd.o \
       aaxmouse.o \
       fontgl.o \
       font16.o \
       font14.o \
       fontx16.o \
       fontx13.o \
       fontx13b.o \
       fontcour.o \
       aarec.o \
       font9.o \
       font8.o \
       fontline.o \
       aacurses.o \
       aacurkbd.o \
       aacurmou.o \
       aalib.o \
       aaflush.o \
       aarender.o \
       aaout.o \
       aastderr.o \
       aain.o \
       aaedit.o \
       aaregist.o \
       aakbdreg.o \
       aagpm.o  \
       aamoureg.o \
       aasave.o \
       aastdout.o \
       aastdin.o \
       aaparse.o



#config.h:
#cp config.autoconf config.h

all:	libaa.a aatest fire savefont info


libaa.a:  $(OBJS)
	#$(CC) $(CFLAGS) $(LFLAGS) -o aview $(OBJS) $(LIBS)
	rm -f libaa.a
	ar rcs libaa.a $(OBJS)
	ranlib libaa.a

aatest: aatest.o libaa.a
	$(CC) $(CFLAGS) $(LFLAGS) -o aatest aatest.o $(LIBS)
fire:   fire.o libaa.a
	$(CC) $(CFLAGS) $(LFLAGS) -o fire fire.o $(LIBS)
savefont:   savefont.o libaa.a
	$(CC) $(CFLAGS) $(LFLAGS) -o savefont savefont.o $(LIBS)
info:   info.o libaa.a
	$(CC) $(CFLAGS) $(LFLAGS) -o info info.o $(LIBS)


install: libaa.a
	./install-sh -c -d $(libdir)
	./install-sh -c -d $(includedir)
	./install-sh -c -d $(infodir)
	./install-sh -c -m 444 libaa.a $(libdir)
	./install-sh -c -m 444 aalib.h $(includedir)
	./install-sh -c -m 444 doc/aalib.info $(infodir)

clean:
	rm -f libaa.a aatest fire info savefont
	rm -f core `find . -name '*.[oas]' -print`
	rm -f core `find . -name '*~' -print`
	rm -f core `find . -type f -name 'core' -print`

distclean:clean
	rm config.h config.log config.status config.cache makefile

#dep:
#	rm -f .depend
#	make .depend
#
#.depend:
#	echo '# Program dependencies' >.depend
#	gcc -I svgalib $(DEFINES) -MM $(patsubst %.o,%.c,$(OBJS)) >>.depend
#include .depend

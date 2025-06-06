# Makefile for XC
# This file was generated by Configure on Mon Jan 31 18:48:22 WET 1994 by root

BINMODE	= 4755
CATMAN	= true
CC	= gcc
CFLAGS	= -O3 -g
GROUP	= uucp
INSDIR	= /opt/bin
LIBDIR	= /opt/lib/xc
LDFLAGS	= -ltermcap
MANDIR	= /opt/man/cat1
MANEXT	= 1
MANMODE	= 0444
OWNER	= uucp

# you should have all of these files:

MANIFEST	=\
	.aus\
	.autocis\
	.callhq\
	.cisdial\
	.exrc\
	.pass\
	.phonelist\
	.rz\
	.sz\
	.temp_opt\
	.uni\
	.xc\
	Configure\
	Copyright\
	Makefile\
	Makefile.in\
	README\
	bsdinst.sh\
	call\
	callback\
	cisdownload\
	myman\
	xc.h.in\
	xc.nro\
	xcb+.c\
	xcdbglog.c\
	xcdial.c\
	xcmain.c\
	xcport.c\
	xcscrpt.c\
	xcsubs.c\
	xcterm.c\
	xcxmdm.c

OBJS =	xcb+.o\
		xcdbglog.o\
		xcdial.o\
		xcmain.o\
		xcport.o\
		xcscrpt.o\
		xcsubs.o\
		xcterm.o\
		xcxmdm.o

all:	xc
	@echo "All targets are up to date"
	@echo "Now you must su to 'root', and then execute 'make install.suid'"

xc:		$(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o xc $(LDFLAGS) 

install.suid: 	xc
	-mv $(INSDIR)/xc $(INSDIR)/xc-
	./bsdinst.sh -c -o $(OWNER) -g $(GROUP) -m $(BINMODE) -s xc $(INSDIR)/xc
	-@ls -l ./xc $(INSDIR)/xc $(INSDIR)/xc-
	@echo
	@echo "The sample xc scripts (.aus .autocis .callhq .cisdial .exrc"
	@echo ".pass .phonelist .rz .sz .temp_opt .uni .xc) should be manually"
	@echo "moved to the $(LIBDIR) directory.  The shell scripts (call"
	@echo "callback cisdownload) should be moved to the $(INSDIR) directory."
	@echo
	@echo "We do not do this automatically, as you may want to recompile xc"
	@echo "with different options, yet not touch these scripts once you've"
	@echo "personalized them with your own passwords and telephone numbers."

install.man:	xc.out
	./bsdinst.sh -c -m $(MANMODE) xc.out $(MANDIR)/xc.$(MANEXT)
	touch install.man

install.scripts:
	if [ ! -d $(LIBDIR) ]; then \
	  mkdir -p $(LIBDIR); \
	fi
	for i in .aus .autocis .callhq .cisdial .exrc .pass .phonelist .rz .sz .temp_opt .uni .xc; do \
	  cp -p $$i $(LIBDIR); \
	done
	if [ ! -d $(INSDIR) ]; then \
	  mkdir -p $(INSDIR); \
	fi
	for i in call callback cisdownload; do \
	  cp -p $$i $(INSDIR); \
	done


xc.out:	xc.man
	if $(CATMAN); then				\
		nroff xc.man | col >xc.out;	\
	else							\
			cat xc.man >xc.out;		\
	fi

xc.man:	myman xc.nro
	cat myman xc.nro | grep -v '^\.tm ' | tbl >xc.man

tooclean:
	rm -f $(OBJS) xc xc.man xc.out core man catman\
		configure.out Makefile.xc xc.h *~ *\#

# Mod by Greg Lehey, LEMIS, 31 January 1994: don't remove config
# info with make clean, use target 'tooclean' above.
clean:
	rm -f $(OBJS) xc xc.man xc.out core man catman\
		 *~ *\#

$(OBJS):	xc.h

xc.h:	xc.h.in Configure
	./Configure

shar:
	shar -awF -L51 -nXC -oXC -s jpr@jpr.com $(MANIFEST)
	@ls -l XC.??

taz:
	tar cf - $(MANIFEST) | compress > XC.TAR.Z
	@ls -l *.Z

tah:
	tar cf - $(MANIFEST) | compress -H > XC.TAR.Z
	@ls -l *.Z

lha:
	lha c XC $(MANIFEST)
	@ls -L XC.A XC.lha

shark:
	shark $(MANIFEST) > XC.SHK
	@ls -l *SHK

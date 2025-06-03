#
#	Simple Makefile for `xneko'
#
#			Written by Kobachan
#

MANDIR=/opt/man
CAT1DIR=cat6
CC            =	gcc

LINKER        =	gcc

TROFF         =	ditroff

VGRIND        =	jvgrind

PRINTER       =	lbp

SHAR          =	xshar

#-------------

DEST          = /opt/bin

CTLIBDIR      = /opt/lib/ctree

BITMAPDIR     = ./bitmaps

#-------------

CFLAGS        =	-O3 -g
# Otherwise:	CFLAGS        =	-DDEBUG

PROGRAM       =	xneko

MAKEFILE      =	Makefile

BITMAPS       = $(BITMAPDIR)/icon.xbm\
		$(BITMAPDIR)/cursor.xbm\
		$(BITMAPDIR)/cursor_mask.xbm\
		$(BITMAPDIR)/space.xbm\
		$(BITMAPDIR)/mati2.xbm\
		$(BITMAPDIR)/jare2.xbm\
		$(BITMAPDIR)/kaki1.xbm\
		$(BITMAPDIR)/kaki2.xbm\
		$(BITMAPDIR)/mati3.xbm\
		$(BITMAPDIR)/sleep1.xbm\
		$(BITMAPDIR)/sleep2.xbm\
		$(BITMAPDIR)/awake.xbm\
		$(BITMAPDIR)/up1.xbm\
		$(BITMAPDIR)/up2.xbm\
		$(BITMAPDIR)/down1.xbm\
		$(BITMAPDIR)/down2.xbm\
		$(BITMAPDIR)/left1.xbm\
		$(BITMAPDIR)/left2.xbm\
		$(BITMAPDIR)/right1.xbm\
		$(BITMAPDIR)/right2.xbm\
		$(BITMAPDIR)/upleft1.xbm\
		$(BITMAPDIR)/upleft2.xbm\
		$(BITMAPDIR)/upright1.xbm\
		$(BITMAPDIR)/upright2.xbm\
		$(BITMAPDIR)/dwleft1.xbm\
		$(BITMAPDIR)/dwleft2.xbm\
		$(BITMAPDIR)/dwright1.xbm\
		$(BITMAPDIR)/dwright2.xbm\
		$(BITMAPDIR)/utogi1.xbm\
		$(BITMAPDIR)/utogi2.xbm\
		$(BITMAPDIR)/dtogi1.xbm\
		$(BITMAPDIR)/dtogi2.xbm\
		$(BITMAPDIR)/ltogi1.xbm\
		$(BITMAPDIR)/ltogi2.xbm\
		$(BITMAPDIR)/rtogi1.xbm\
		$(BITMAPDIR)/rtogi2.xbm

SRCS          =	xneko.c

OBJS          =	xneko.o

DOCS          =	README xneko.map

#-------------

all:		$(PROGRAM)

$(PROGRAM):	$(OBJS)
		$(LINKER) $(OBJS) -L/usr/ucblib -lucb -lm -lX11 -lnsl -o $(PROGRAM)

$(OBJS):	$(BITMAPS) $(MAKEFILE)

lint:;		lint -Dlint $(SRCS)

shar:;		$(SHAR) -v $(DOCS) $(MAKEFILE) $(SRCS) $(BITMAPS) > shar.work
		grep '^#' shar.work 		>  shar.head
		grep '^[^#]' shar.work 		>  shar.tail
		echo "if test ! -d bitmaps"	>  shar.middle
		echo "then"			>> shar.middle
		echo "    mkdir bitmaps"	>> shar.middle
		echo "fi"			>> shar.middle
		cat shar.head shar.middle shar.tail > xneko.shar
		rm -f shar.work shar.head shar.middle shar.tail

print:;		$(VGRIND) $(SRCS) | $(PRINTER) -n
		ctags -v $(SRCS) | sort > index
		$(VGRIND) -x index | $(PRINTER) -n

install:	$(PROGRAM)
		@echo Installing $(PROGRAM) in $(DEST)
		@install -o bin -g bin -m 755 -s $(PROGRAM) $(DEST)

install-sysv:	install
		cat README >$(MANDIR)/$(CAT1DIR)/xneko.1
		chown bin $(MANDIR)/$(CAT1DIR)/xneko.1
		chgrp bin $(MANDIR)/$(CAT1DIR)/xneko.1
		chmod 644 $(MANDIR)/$(CAT1DIR)/xneko.1

xneko.map:	$(SRCS)
		@ctree -UDEBUG -n\
		-ffunc-lib $(CTLIBDIR)/clib.cdb\
		-ffunc-lib $(CTLIBDIR)/syscall.cdb\
		-ffunc-lib $(CTLIBDIR)/X11lib.cdb $(SRCS)

ctree:;		@ctree -UDEBUG -n\
		-ffunc-lib $(CTLIBDIR)/clib.cdb\
		-ffunc-lib $(CTLIBDIR)/syscall.cdb\
		-ffunc-lib $(CTLIBDIR)/X11lib.cdb $(SRCS)

clean:;		rm -f $(OBJS) index\
		shar.work shar.head shar.middle shar.tail xneko.shar

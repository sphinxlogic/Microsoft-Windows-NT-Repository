#=============================================================================
#
# Name - simple.mak
#
# Version:	1.12
#
# ccsid:	@(#)simple.mak	1.12 - 8/12/92 12:39:14
# from: 	ccs/s.simple.mak
# date: 	8/12/92 14:15:40
#
# Description: make file for xgrabsc.  Use "make -f simple.mak"
#
#=============================================================================

#CC=gcc
#CFLAGS = -g -DNO_RLE_CHECKS -DMEMCPY
#CFLAGS = -g
CFLAGS = -O

#LDFLAGS = -Bstatic
LDFLAGS =

# change INSTALL_PATH to the directory in which you want xgrabsc installed
INSTALL_PATH    = /usr/bin/X11

# change XAPPLOADDIR to the directory in which your want xgrab app-defaults
# installed
XAPPLOADDIR = /usr/lib/X11/app-defaults

# change MAN_PATH to point to your man page top directory
MAN_PATH        = /usr/man
# change MAN_EXT to the section for xgrabsc
MAN_EXT         = n

PROGRAMS = xgrabsc xgrab

all: $(PROGRAMS)

xgrabsc: xgrabsc.o
	rm -f xgrabsc
	$(CC) $(LDFLAGS) $(CFLAGS) -o xgrabsc xgrabsc.o -lX11

xgrabsc.o: xgrabsc.c ps_color.h checkvm.h patchlevel.h cpyright.h \
  process.hc get.hc mem.hc convert.hc write.hc cmdopts.h config.h

xgrab: xgrab.o
	rm -f xgrab
	$(CC) $(LDFLAGS) $(CFLAGS) -o xgrab xgrab.o -lXaw -lXt -lXmu -lXext -lX11


xgrab.o: xgrab.c xgrab_ad.h config.h

xgrab_ad.h: XGrab.ad
	rm -f xgrab_ad.h
	sed -n '1,/! ====/p' XGrab.ad | sed -n -f ad2c.sed >xgrab_ad.h

install::
	install -c -s xgrabsc $(INSTALL_PATH)
	install -c -s xgrab $(INSTALL_PATH)
	install -c -s XGrab.ad $(XAPPLOADDIR)/XGrab

install.man::
	install -c -m 644 xgrabsc.man \
		$(MAN_PATH)/man$(MAN_EXT)/xgrabsc.$(MAN_EXT)
	install -c -m 644 xgrab.man \
		$(MAN_PATH)/man$(MAN_EXT)/xgrab.$(MAN_EXT)

clean::
	rm -f core *.o xgrabsc xgrab *.log xgrab_ad.h test*


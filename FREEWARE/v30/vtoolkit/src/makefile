# makefile for MOTIF VToolkit

#******************************************************************************
# VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
# (c) Steve Klein							      *
# Unpublished rights reserved under the copyright laws of the United States.  *
#******************************************************************************

.SUFFIXES: .uil .uid

UIL	= /usr/bin/uil

CC	= cc
CFLAGS	= -DMOTIF -I/usr/include/X11 -I/usr/include

XLIBS	= -lMrm -lXm -lXt -lX11 	

LDLIBS	= libVtk.a $(XLIBS)
LDFLAGS	= -s

VWIDGETS = tlist.o vautorepeat.o vdrag.o vfake.o vframe.o vheader.o vhist.o \
	   vlist.o vtkcursor.o vtkmainloop.o vtksavegeometry.o vtkutil.o

TESTS	= compvlist simpletestvlist testtlist testvhist testvlist \
	  vcolorizer vcountdown vdragroot vbreak

TESTUIDS = compvlist.uid simpletestvlist.uid testtlist.uid testvhist.uid \
	   testvlist.uid vcolorizer.uid vcountdown.uid

libVtk.a: $(VWIDGETS)
	ar r $@ $(VWIDGETS)
	ranlib $@

all: libVtk.a $(TESTS) $(TESTUIDS)

clean:
	rm -f *.o *.uid $(TESTS) libVtk.a

compvlist:		compvlist.c libVtk.a compvlist.uid
simpletestvlist:	simpletestvlist.c libVtk.a simpletestvlist.uid
testtlist:		testtlist.c libVtk.a testtlist.uid
testvhist:		testvhist.c libVtk.a testvhist.uid
testvlist:		testvlist.c libVtk.a testvlist.uid
vcolorizer:		vcolorizer.c libVtk.a vcolorizer.uid
vcountdown:		vcountdown.c libVtk.a vcountdown.uid
vdragroot:		vdragroot.c libVtk.a
vbreak:			vbreak.c libVtk.a

compvlist.uid:		compvlist.uil
simpletestvlist.uid: 	simpletestvlist.uil
testtlist.uid: 		testtlist.uil
testvhist.uid: 		testvhist.uil
testvlist.uid: 		testvlist.uil
vcolorizer.uid: 	vcolorizer.uil
vcountdown.uid: 	vcountdown.uil

.uil.uid:
	$(UIL) -o $*.uid $<

.c$<:
	$(CC) $(LDFLAGS) $@ $@.c $(LDLIBS)

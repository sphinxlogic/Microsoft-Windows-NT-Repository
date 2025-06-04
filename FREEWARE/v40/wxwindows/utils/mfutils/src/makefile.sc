# Symantec C++ makefile for the MFUTILS library
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
!include $(WXDIR)\src\makesc.env

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

MFDIR = $(WXDIR)\utils\mfutils
MFINC = $(MFDIR)\src
MFLIB = $(MFDIR)\lib\mfutils.lib

CC=sc
CFLAGS = -o -ml -W -Dwx_msw

INCLUDE=$(BASEINC);$(MSWINC)

OBJS = mfutils.obj

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

$(MFLIB): $(OBJS)
 	-del $(MFLIB)
	*lib $(MFLIB) y $(OBJS), nul;

$(OBJS): $(MFINC)\mfutils.h

clean:
        -del *.obj
	-del $(MFLIB)

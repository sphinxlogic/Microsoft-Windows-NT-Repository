# Makefile : Builds ITSYBITS library
# for Symantec C++

# Set WXDIR for your system
WXDIR = $(WXWIN)

!include $(WXDIR)\src\makesc.env

WXLIB = $(WXDIR)\lib\wx.lib
ITSYDIR = $(WXDIR)\contrib\itsybits
ITSYLIB = $(FAFADIR)\itsy.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

INCLUDE=$(BASEINC);$(MSWINC);$(WXDIR)\contrib\itsybits


OBJS = itsybits.obj

.c.obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

all: $(ITSYLIB)

$(ITSYLIB): $(OBJS)
        -del $(ISTYLIB)
	*lib $(ITSYLIB) y $(OBJS), nul;

itsybits.obj: itsybits.h itsybits.$(SRCSUFF)

clean:
        -del *.obj
	-del $(ITSYLIB)

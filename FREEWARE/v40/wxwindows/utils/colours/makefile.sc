# Symantec C++ makefile for Colour Sampler
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
!include $(WXDIR)\src\makesc.env

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

CC=sc
RC=rc
CFLAGS = -o -ml -W -Dwx_msw
LDFLAGS = -ml -W

INCLUDE=$(BASEINC);$(MSWINC)

LIBS=$(WXLIB) libw.lib commdlg.lib shell.lib

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

colours.exe: colours.obj colours.def colours.res
	*$(CC) $(LDFLAGS) -o$@ colours.obj colours.def $(LIBS)
        *$(RC) -k colours.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws


# Symantec C++ makefile for animation example
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

OBJECTS=animate.obj tool3d.obj

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

animate.exe: $(OBJECTS) animate.def animate.res
	*$(CC) $(LDFLAGS) -o$@ $(OBJECTS) animate.def $(LIBS)
        *$(RC) -k animate.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws


# Symantec C++ makefile for form example
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)

!include $(WXDIR)\src\makesc.env

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

INCLUDE=$(BASEINC);$(MSWINC)

LIBS=$(WXLIB) libw.lib commdlg.lib shell.lib

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

form.exe: form.obj form.def form.res
	*$(CC) $(LDFLAGS) -o$@ form.obj form.def $(LIBS)
	*$(RC) -k form.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws

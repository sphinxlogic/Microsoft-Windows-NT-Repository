# Symantec C++ makefile for ipc example
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

LIBS=$(WXLIB) libw.lib commdlg.lib ddeml.lib shell.lib

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

all: client.exe server.exe

client.exe: client.obj client.def client.res
	*$(CC) $(LDFLAGS) -o$@ client.obj client.def $(LIBS)
	*$(RC) -k client.res

server.exe: server.obj server.def server.res
	*$(CC) $(LDFLAGS) -o$@ server.obj server.def $(LIBS)
	*$(RC) -k server.res

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws

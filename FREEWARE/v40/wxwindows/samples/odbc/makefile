WXDIR = c:\libs\wx161
THISDIR = .
WXLIB = $(WXDIR)\lib\wx.lib
LIBS=$(WXLIB) cw32 import32 fafa itsy ctl3d32.lib odbc32.lib
INC=-I$(WXDIR)\include\base -I$(WXDIR)\include\msw
CFG=$(WXDIR)\src\wxwin32.cfg
BCCDIR = c:\sp\bc
BCCLIBPATH = $(BCCDIR)\lib

FINAL=0
!if "$(FINAL)" == "0"
LINKFLAGS=/v /Tpe /L$(WXDIR)\lib;$(BCCDIR)\lib
OPT = -Od
DEBUG_FLAGS= -v
!else
LINKFLAGS=/Tpe /L$(WXDIR)\lib;$(BCCDIR)\lib
OPT = -O2
DEBUG_FLAGS=
!endif
CPPFLAGS=$(DEBUG_FLAGS) $(OPT) @$(CFG)

OBJECTS = wxdb.obj

wxdb:    wxdb.exe

all:    wxdb.exe

wxdb.exe:    $(WXLIB) wxdb.obj wxdb.def wxdb.res
        tlink32 $(LINKFLAGS) @&&!
$(BCCLIBPATH)\c0w32.obj wxdb.obj
wxdb
nul
$(LIBS)
wxdb.def
!
        brc32 -K wxdb.res

.cc.obj:
	bcc32 $(CPPFLAGS) -c {$< }

wxdb.res :      wxdb.rc $(WXDIR)\include\msw\wx.rc
    brc32 -r /i$(BCCDIR)\include /i$(WXDIR)\include\msw /i$(WXDIR)\contrib\fafa wxdb

clean:
        -erase *.obj
        -erase *.exe
        -erase *.res
        -erase *.map
        -erase *.rws

# wxHelp makefile

WXDIR = ..\..\..

EXTRACPPFLAGS =

!include $(WXDIR)\src\makewat.env

EXTRALIBS   = $(WATLIBDIR)\ctl3d32.lib ..\..\hytext\lib\hytext.lib $(WATLIBDIR)\odbc32.lib
THISDIR = $(WXDIR)\utils\wxhelp\src
IFLAGS  = -i=$(WXINC) -i=$(WXBASEINC) -i=..\..\hytext\src

NAME = wxhelp
LNK = $(name).lnk

OBJECTS = wxhelp.obj

all: hytext erasepch $(name).exe

hytext: .SYMBOLIC
    cd ..\..\hytext\src
    wmake  -f makefile.wat all
    cd $(THISDIR)

$(name).exe : $(OBJECTS) $(name).res $(LNK) $(WXDIR)\lib\wx$(LEVEL).lib
    wlink @$(LNK)
    $(BINDCOMMAND) $(name).res

$(name).res :      $(name).rc $(WXDIR)\include\msw\wx.rc
     $(RC) $(RESFLAGS1) $(name).rc

$(LNK) : makefile.wat
    %create $(LNK)
    @%append $(LNK) $(DEBUGINFO)
    @%append $(LNK) system $(LINKOPTION)
    @%append $(LNK) $(MINDATA)
    @%append $(LNK) $(MAXDATA)
    @%append $(LNK) $(STACK)
    @%append $(LNK) name $(name)
    @%append $(LNK) file $(WXDIR)\lib\wx$(LEVEL).lib
    @for %i in ($(EXTRALIBS)) do @%append $(LNK) file %i
    @for %i in ($(OBJECTS)) do @%append $(LNK) file %i

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch *.lib *.lbc *.res



# wxHelp2 makefile

WXDIR = ..\..\..

EXTRACPPFLAGS =

!include $(WXDIR)\src\makewat.env

EXTRALIBS   = $(WATLIBDIR)\ctl3d32.lib $(WATLIBDIR)\odbc32.lib
THISDIR = $(WXDIR)\utils\wxhelp2\src
IFLAGS  = -i=$(WXINC) -i=$(WXBASEINC) -i=..\..\wxweb\src

NAME = wxhelp
LNK = $(name).lnk

OBJECTS = wxhelp.obj

all: erasepch $(name).exe

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
    @%append $(LNK) file $(WXDIR)\utils\wximage\lib\wxima.lib
    @%append $(LNK) file $(WXDIR)\utils\wxweb\lib\wxhtml.lib
    @for %i in ($(EXTRALIBS)) do @%append $(LNK) file %i
    @for %i in ($(OBJECTS)) do @%append $(LNK) file %i

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch *.lib *.lbc *.res



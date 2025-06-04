# Tex2RTF makefile

WXDIR = ..\..\..

EXTRACPPFLAGS =

!include $(WXDIR)\src\makewat.env

EXTRALIBS   = $(WATLIBDIR)\ctl3d32.lib # $(WATLIBDIR)\odbc32.lib
THISDIR = $(WXDIR)\utils\tex2rtf\src
IFLAGS  = -i=$(WXINC) -i=$(WXBASEINC) -i=..\..\wxhelp\src

NAME = tex2rtf
LNK = $(name).lnk

OBJECTS = tex2rtf.obj tex2any.obj texutils.obj rtfutils.obj htmlutil.obj xlputils.obj readshg.obj table.obj

all: erasepch tex2rtf.exe

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



WXDIR = ..\..\..

NOPRECOMP=1

!include $(WXDIR)\src\makewat.env

LIBTARGET = ..\lib\wxchart.lib
WXCHARTLIB = $(LIBTARGET)
IFLAGS      = -i=$(WXINC) -i=$(WXBASEINC)
EXTRACPPFLAGS =
NAME        = test
LNK = $(name).lnk

OBJECTS = lbchart.obj dataset.obj
TESTOBJECTS = shapes.obj test.obj

all: $(OBJECTS) $(LIBTARGET)

.cpp.obj: # $< # .AUTODEPEND
    *$(CCC) $(CPPFLAGS) $(IFLAGS) $<

$(LIBTARGET): $(OBJECTS)
	*wlib /b /c /n /P=256 $(LIBTARGET) $(OBJECTS)

$(name).exe : $(TESTOBJECTS) $(WXCHARTLIB) $(name).res $(LNK) $(WXDIR)\lib\wx$(LEVEL).lib
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
    @%append $(LNK) file $(WXCHARTLIB)
    @for %i in ($(EXTRALIBS)) do @%append $(LNK) file %i
    @for %i in ($(TESTOBJECTS)) do @%append $(LNK) file %i

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch *.lib *.lbc



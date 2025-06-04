WXDIR = ..\..\..

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib
LIBTARGET = ..\lib\wxgrid.lib
IFLAGS      = -i=$(WXINC) -i=$(WXBASEINC)
EXTRACPPFLAGS =
NAME        = wxgridg
LNK = $(name).lnk

OBJECTS = $(name).obj

all: $(OBJECTS) $(LIBTARGET)

demo: test.exe

$(LIBTARGET): $(OBJECTS)
	*wlib /b /c /n /P=256 $(LIBTARGET) $(OBJECTS)

test.exe : test.obj test.res $(LNK) $(WXLIB)\wx$(LEVEL).lib $(LIBTARGET)
    wlink @$(LNK)
    $(BINDCOMMAND) test.res

test.obj:   test.$(SRCSUFF)

test.res :      test.rc $(WXDIR)\include\msw\wx.rc
     $(RC) $(RESFLAGS1) test.rc

$(LNK): makefile.wat
    %create $(LNK)
    @%append $(LNK) debug all
    @%append $(LNK) system $(LINKOPTION)
    @%append $(LNK) $(MINDATA)
    @%append $(LNK) $(MAXDATA)
    @%append $(LNK) $(STACK)
    @%append $(LNK) name test
    @%append $(LNK) file $(WXLIB)\wx$(LEVEL).lib
    @%append $(LNK) file $(LIBTARGET)
    @%append $(LNK) file test.obj
    @for %i in ($(EXTRALIBS)) do @%append $(LNK) file %i

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch *.lib *.lbc



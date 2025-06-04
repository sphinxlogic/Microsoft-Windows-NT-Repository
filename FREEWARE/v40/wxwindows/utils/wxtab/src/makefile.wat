WXDIR = ..\..\..

NOPRECOMP = 1

!include $(WXDIR)\src\makewat.env

WXLIB=$(WXDIR)\lib
LIBTARGET = ..\lib\wxtab.lib
IFLAGS      = -i=$(WXINC) -i=$(WXBASEINC)
EXTRACPPFLAGS =
NAME        = wxtab

OBJECTS = $(name).obj
TESTOBJECTS = test.obj
LNK = $(name).lnk

all: $(OBJECTS) $(LIBTARGET)

test: test.exe

test.obj:   test.$(SRCSUFF) test.h wxtab.h

test.exe : $(TESTOBJECTS) test.res $(LNK) $(WXLIB)\wx$(LEVEL).lib
    wlink @$(LNK)
    $(BINDCOMMAND) test.res

test.res :      test.rc $(WXDIR)\include\msw\wx.rc
     $(RC) $(RESFLAGS1) test.rc

$(LNK) : makefile.wat
    %create $(LNK)
    @%append $(LNK) debug all
    @%append $(LNK) system $(LINKOPTION)
    @%append $(LNK) $(MINDATA)
    @%append $(LNK) $(MAXDATA)
    @%append $(LNK) $(STACK)
    @%append $(LNK) name test
    @%append $(LNK) file $(WXLIB)\wx$(LEVEL).lib
    @%append $(LNK) file $(LIBTARGET)
    @for %i in ($(EXTRALIBS)) do @%append $(LNK) file %i
    @for %i in ($(TESTOBJECTS)) do @%append $(LNK) file %i

$(LIBTARGET): $(OBJECTS)
	*wlib /b /c /n /P=256 $(LIBTARGET) $(OBJECTS)

clean:   .SYMBOLIC
    -erase $(LIBTARGET) *.obj *.bak *.err *.pch *.lib *.lbc *.res *.exe



WXDIR = ..\..\..

EXTRACPPFLAGS = /DUSE_DEFINE

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib
LIBTARGET = ..\lib\prologio.lib

OBJECTS = y_tab.obj read.obj # prorpc.obj
TESTOBJECTS = test.obj
NAME = test
LNK = $(name).lnk

all: erasepch $(OBJECTS) $(LIBTARGET)

test: test.exe

test.obj:   test.$(SRCSUFF)

y_tab.obj:  y_tab.c lex_yy.c

y_tab.c:    dosyacc.c
    copy dosyacc.c y_tab.c

lex_yy.c:    doslex.c
    copy doslex.c lex_yy.c

$(LIBTARGET): $(OBJECTS)
	wlib /b /c /n /P=256 $(LIBTARGET) $(OBJECTS)

$(name).exe : $(TESTOBJECTS) $(name).res $(LNK) $(WXLIB)\wx$(LEVEL).lib
    wlink @$(LNK)
    $(BINDCOMMAND) $(name).res

$(name).res :      $(name).rc $(WXDIR)\include\msw\wx.rc
     $(RC) $(RESFLAGS1) $(name).rc

$(LNK) : makefile.wat
    %create $(LNK)
    @%append $(LNK) debug all
    @%append $(LNK) system $(LINKOPTION)
    @%append $(LNK) $(MINDATA)
    @%append $(LNK) $(MAXDATA)
    @%append $(LNK) $(STACK)
    @%append $(LNK) name $(name)
    @%append $(LNK) file $(WXLIB)\wx$(LEVEL).lib
#    @%append $(LNK) file $(LIBTARGET)
    @for %i in ($(EXTRALIBS)) do @%append $(LNK) file %i
    @for %i in ($(TESTOBJECTS)) do @%append $(LNK) file %i

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch $(LIBTARGET) *.lbc



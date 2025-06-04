# wxPoem makefile

WXDIR = ..\..

!include $(WXDIR)\src\makewat.env

THISDIR = $(WXDIR)\samples\wxpoem

NAME = wxpoem
LNK = $(name).lnk

IFLAGS      = -i=$(WXINC) -i=$(WXBASEINC)

PROGOBJECTS = wxpoem.obj

all: $(name).exe

wx: .SYMBOLIC
    cd $(WXDIR)\src\msw
    wmake -f makefile.wat all
    cd $(THISDIR)

$(name).exe : $(PROGOBJECTS) $(name).res $(LNK) $(WXDIR)\lib\wx$(LEVEL).lib
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
    @for %i in ($(PROGOBJECTS)) do @%append $(LNK) file %i

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch *.exe *.lbc *.res



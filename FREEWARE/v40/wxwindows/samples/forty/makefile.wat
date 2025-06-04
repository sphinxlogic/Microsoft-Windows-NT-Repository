#
# Makefile for WATCOM
#

WXDIR = ..\.. 

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib
NAME = forty
LNK = $(name).lnk
OBJS = $(name).obj canvas.obj card.obj game.obj pile.obj playerdg.obj scoredg.obj scorefil.obj
THISDIR = $(WXDIR)\samples\forty

PRECOMP=

all:    $(name).exe

$(name).exe : $(OBJS) $(name).res $(LNK) $(WXLIB)\wx$(LEVEL).lib
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
    @for %i in ($(EXTRALIBS)) do @%append $(LNK) file %i
    @for %i in ($(OBJS)) do @%append $(LNK) file %i
    
wx:     .SYMBOLIC
    cd $(WXDIR)\src\msw
    wmake -f makefile.wat all
    cd $(THISDIR)

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch *.lib *.lnk *.res *.exe *.rex


#
# Makefile for WATCOM
#
# Created by D.Chubraev, chubraev@iem.ee.ethz.ch
# 8 Nov 1994
#

WXDIR = ..\.. 

!include $(WXDIR)\src\makewat.env

WXLIB = $(WXDIR)\lib
NAME = ipc
LNK1 = client.lnk
LNK2 = server.lnk
CLIENTOBJS = client.obj 
SERVEROBJS = server.obj 
THISDIR = $(WXDIR)\samples\ipc

PRECOMP=

all:    client.exe server.exe

client.exe : $(CLIENTOBJS) client.res $(LNK1) $(WXLIB)\wx$(LEVEL).lib
    wlink @$(LNK1)
    $(BINDCOMMAND) client.res

client.res :      client.rc $(WXDIR)\include\msw\wx.rc
     $(RC) $(RESFLAGS1) client.rc

server.res :      server.rc $(WXDIR)\include\msw\wx.rc
     $(RC) $(RESFLAGS1) server.rc

server.exe : $(SERVEROBJS) server.res $(LNK2) $(WXLIB)\wx$(LEVEL).lib
    wlink @$(LNK2)
    $(BINDCOMMAND) server.res

$(LNK1) : makefile.wat
    %create $(LNK1)
    @%append $(LNK1) debug all
    @%append $(LNK1) system $(LINKOPTION)
    @%append $(LNK1) $(MINDATA)
    @%append $(LNK1) $(MAXDATA)
    @%append $(LNK1) $(STACK)
    @%append $(LNK1) name client
    @%append $(LNK1) file $(WXLIB)\wx$(LEVEL).lib
    @for %i in ($(EXTRALIBS)) do @%append $(LNK1) file %i
    @for %i in ($(OBJS)) do @%append $(LNK1) file %i
    
$(LNK2) : makefile.wat
    %create $(LNK2)
    @%append $(LNK2) debug all
    @%append $(LNK2) system $(LINKOPTION)
    @%append $(LNK2) $(MINDATA)
    @%append $(LNK2) $(MAXDATA)
    @%append $(LNK2) $(STACK)
    @%append $(LNK2) name server
    @%append $(LNK2) file $(WXLIB)\wx$(LEVEL).lib
    @for %i in ($(EXTRALIBS)) do @%append $(LNK2) file %i
    @for %i in ($(OBJS)) do @%append $(LNK2) file %i
    
wx:     .SYMBOLIC
    cd $(WXDIR)\src\msw
    wmake -f makefile.wat all
    cd $(THISDIR)

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch *.lib *.lnk *.res *.exe *.rex


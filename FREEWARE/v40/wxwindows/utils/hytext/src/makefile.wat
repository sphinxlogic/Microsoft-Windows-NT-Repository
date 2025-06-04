# hyText makefile

WXDIR = ..\..\..

NOPRECOMP=1

!include $(WXDIR)\src\makewat.env

HYTEXTLIB = $(WXDIR)\utils\hytext\lib\hytext.lib
THISDIR = $(WXDIR)\utils\hytext\src

NAME = hytext
LNK = $(name).lnk

IFLAGS      = -i=$(WXINC) -i=$(WXBASEINC)

OBJECTS = hytext.obj

all: $(HYTEXTLIB)

$(HYTEXTLIB): $(OBJECTS)
	*wlib /b /c /n /P=256 $(HYTEXTLIB) $(OBJECTS)

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch $(HYTEXTLIB) *.lbc




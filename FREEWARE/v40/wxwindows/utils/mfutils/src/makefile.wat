# MFUTILS makefile

WXDIR = ..\..\..

NOPRECOMP=1

!include $(WXDIR)\src\makewat.env

MFUTILSLIB = $(WXDIR)\utils\mfutils\lib\mfutils.lib
THISDIR = $(WXDIR)\utils\mfutils\src

NAME = mfutils
LNK = $(name).lnk

IFLAGS      = -i=$(WXINC) -i=$(WXBASEINC)

OBJECTS = $(name).obj

all: $(MFUTILSLIB)

$(MFUTILSLIB): $(OBJECTS)
	*wlib /b /c /n /P=256 $(MFUTILSLIB) $(OBJECTS)

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch $(MFUTILSLIB) *.lbc




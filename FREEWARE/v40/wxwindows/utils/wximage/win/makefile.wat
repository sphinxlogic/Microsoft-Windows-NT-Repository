WXDIR = ..\..\..

!include $(WXDIR)\src\makewat.env

LIBTARGET = ..\lib\wxima.lib
IFLAGS      = -i=$(WXINC) -i=$(WXBASEINC)
EXTRACPPFLAGS =

OBJECTS = dibutils.obj gifdecod.obj imabmp.obj imagif.obj wbima.obj wxima.obj

all: erasepch $(OBJECTS) $(LIBTARGET)

$(LIBTARGET): $(OBJECTS)
	*wlib /b /c /n /P=256 $(LIBTARGET) $(OBJECTS)

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch $(LIBTARGET) *.lbc



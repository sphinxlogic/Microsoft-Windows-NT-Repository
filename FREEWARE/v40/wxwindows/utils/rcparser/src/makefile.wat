WXDIR = ..\..\..

!include $(WXDIR)\src\makewat.env

LIBTARGET = ..\lib\rcparser.lib

OBJECTS = curico.obj rcobject.obj rcparser.obj

all: erasepch $(OBJECTS) $(LIBTARGET)

$(LIBTARGET): $(OBJECTS)
	wlib /b /c /n /P=256 $(LIBTARGET) $(OBJECTS)

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch $(LIBTARGET) *.lbc

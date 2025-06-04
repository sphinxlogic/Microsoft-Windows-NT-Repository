# Symantec C++ makefile for the rcparser library

WXDIR = $(WXWIN)
!include $(WXDIR)\src\makesc.env

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

CTL3DLIB = $(WXDIR)\contrib\ctl3d\ctl3dv2.lib
FAFALIB = $(WXDIR)\contrib\fafa\fafa.lib
ITSYLIB = $(WXDIR)\contrib\itsybits\itsy.lib

RCPARSEDIR = $(WXDIR)\utils\rcparser
RCPARSEINC = $(RCPARSEDIR)\src
RCPARSELIB = $(RCPARSEDIR)\lib\rcparser.lib

DOCDIR = $(RCPARSEDIR)\docs
SRCDIR = $(RCPARSEDIR)\src

# Default is to output RTF for WinHelp
WINHELP=-winhelp

CC=sc
CFLAGS = -o -ml -W -Dwx_msw

INCLUDE=$(BASEINC);$(MSWINC)

OBJS = rcparser.obj rcobject.obj curico.obj

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

$(RCPARSELIB): $(OBJS)
 	-del $(RCPARSELIB)
	*lib $(RCPARSELIB) y $(OBJS), nul;

$(OBJS): $(RCPARSEINC)\rcparser.h $(RCPARSEINC)\rcobject.h $(RCPARSEINC)\curico.h $(RCPARSEINC)\cric_priv.h $(RCPARSEINC)\wxcurico.h

clean:
        -del *.obj
	-del $(RCPARSELIB)

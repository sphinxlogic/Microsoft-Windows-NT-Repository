#!/binb/wmake.exe
#
# File:		makefile.wat
# Author:	David Claughton <David@dclaugh.demon.co.uk>
# Created:	11 Nov 1995
# RCS_ID        $Id$
#
# Makefile : Builds WXBuilder for Windows 95, NT & win32s
# using Watcom C++

WXDIR = ../../..

!include $(WXDIR)/src/makewat.env

WXBUILDDIR = ..

WXLIB = $(WXDIR)\lib
NAME = wxbuild
LNK = $(name).lnk

PROLOGDIR = $(WXDIR)\utils\prologio
PROLOGINC = $(PROLOGDIR)\src
PROLOGLIB = $(PROLOGDIR)\lib\proio.lib

TREEDIR = $(WXDIR)\utils\wxtree
TREEINC = $(TREEDIR)\src
TREELIB = $(TREEDIR)\lib\wxtree.lib

RCPARSERDIR = $(WXDIR)\utils\rcparser
RCPARSERINC = $(RCPARSERDIR)\src

DIBDIR = $(WXDIR)\utils\dib
DIBINC = $(DIBDIR)

OBJDIR = .
OBJSUFF = obj

OBJECTS = $(OBJDIR)\wxbuild.$(OBJSUFF) 	&
	  $(OBJDIR)\bapp.$(OBJSUFF) 	&
	  $(OBJDIR)\bmenu.$(OBJSUFF)	&
	  $(OBJDIR)\bframe.$(OBJSUFF)	&
	  $(OBJDIR)\bsubwin.$(OBJSUFF)	&
 	  $(OBJDIR)\bgencpp.$(OBJSUFF)	&
 	  $(OBJDIR)\namegen.$(OBJSUFF)	&
	  $(OBJDIR)\bitem1.$(OBJSUFF)	&
	  $(OBJDIR)\bitem2.$(OBJSUFF)	&
	  $(OBJDIR)\bwin.$(OBJSUFF)	&
	  $(OBJDIR)\btoolbar.$(OBJSUFF) &
   	  $(OBJDIR)\bactions.$(OBJSUFF)	&
	  $(OBJDIR)\btree.$(OBJSUFF)	&
	  $(OBJDIR)\bgdi.$(OBJSUFF)	&
	  $(OBJDIR)\rcload.$(OBJSUFF)

EXTRAIFLAGS = -i=$(PROLOGINC) -i=$(TREEINC) -i=$(RCPARSERINC) -i=$(DIBINC)

# Add Extra Include Directories to Search.
IFLAGS += $(EXTRAIFLAGS)

# Add WxBuilder Specific Libs To EXTRALIBS.
EXTRALIBS += $(TREELIB)

all:    $(name).exe

$(name).exe : $(OBJECTS) $(name).res $(LNK) $(WXLIB)\wx$(LEVEL).lib
    wlink @$(LNK)
    $(BINDCOMMAND) $(name).res

$(name).res :      $(name).rc
     $(RC) $(RESFLAGS1) /i. $(name).rc

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
    @for %i in ($(OBJECTS)) do @%append $(LNK) file %i

clean:   .SYMBOLIC
    -erase *.obj *.bak *.err *.pch *.lib *.lnk *.res *.exe *.rex

# Symantec C++ makefile for the PROLOGIO library
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
!include $(WXDIR)\src\makesc.env

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

PROLOGDIR = $(WXDIR)\utils\prologio
PROLOGINC = $(PROLOGDIR)\src
PROLOGLIB = $(PROLOGDIR)\lib\prologio.lib

DOCDIR = $(PROLOGDIR)\docs
SRCDIR = $(PROLOGDIR)\src

# Default is to output RTF for WinHelp
WINHELP=-winhelp

CC=sc
RC=rc
CFLAGS = -Jm -w -o -ml -W -Dwx_msw
LDFLAGS = -ml -W

INCLUDE=$(BASEINC);$(MSWINC)

LIBS=$(PROLOGLIB) $(WXLIB) libw.lib commdlg.lib shell.lib

OBJS = read.obj y_tab.obj # prorpc.obj

.c.obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

$(PROLOGLIB): $(OBJS)
 	-del $(PROLOGLIB)
	*lib $(PROLOGLIB) y $(OBJS), nul;

$(OBJS): $(PROLOGINC)\read.h $(PROLOGINC)\expr.h

y_tab.c:      parser.y
        bison -y parser.y

lex_yy.c:     lexer.l
        flex -8 -t lexer.l > lex_yy.c

test.exe: test.obj test.def test.res
	*$(CC) $(LDFLAGS) -o$@ test.obj test.def $(LIBS)
	*$(RC) -k test.res

# Making documents
docs:   $(DOCDIR)/prologio.hlp
rtf:    $(DOCDIR)/prologio.rtf
hlp:    $(DOCDIR)/prologio.hlp

$(DOCDIR)/prologio.hlp:         $(DOCDIR)/prologio.rtf $(DOCDIR)/prologio.hpj
        cd $(DOCDIR)
        -erase prologio.ph
        hc prologio
        cd $(SRCDIR)

$(DOCDIR)/prologio.rtf:         $(DOCDIR)/manual.tex $(DOCDIR)/classes.tex
        cd $(DOCDIR)
        -wx tex2rtf $(DOCDIR)\manual.tex $(DOCDIR)\prologio.rtf -twice $(WINHELP)
        cd $(SRCDIR)

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del $(PROLOGLIB)

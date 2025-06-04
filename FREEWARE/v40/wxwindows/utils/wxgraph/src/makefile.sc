# Symantec C++ makefile for the graph library
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
!include $(WXDIR)\src\makesc.env

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

GRAPHDIR = $(WXDIR)\utils\wxgraph
GRAPHINC = $(GRAPHDIR)\src
GRAPHLIB = $(GRAPHDIR)\lib\wxgraph.lib

DOCDIR = $(GRAPHDIR)\docs
SRCDIR = $(GRAPHDIR)\src

# Default is to output RTF for WinHelp
WINHELP=-winhelp

CC=sc
RC=rc
CFLAGS = -o -ml -W -Dwx_msw
LDFLAGS = -ml -W

INCLUDE=$(BASEINC);$(MSWINC)

LIBS=$(GRAPHLIB) $(WXLIB) libw.lib commdlg.lib shell.lib

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

$(GRAPHLIB): wxgraph.obj
 	-del $(GRAPHLIB)
	*lib $(GRAPHLIB) y wxgraph.obj, nul;

wxgraph.obj: wxgraph.h wxgraphp.h wxgraph.$(SRCSUFF)

test.exe: test.obj test.def test.res
	*$(CC) $(LDFLAGS) -o$@ test.obj test.def $(LIBS)
	*$(RC) -k test.res

test.obj: test.h wxgraph.h test.$(SRCSUFF)

# Making documents
docs:   hlp xlp
hlp:    $(DOCDIR)/wxgraph.hlp
xlp:    $(DOCDIR)/wxgraph.xlp
rtf:    $(DOCDIR)/wxgraph.rtf

$(DOCDIR)/wxgraph.hlp:         $(DOCDIR)/wxgraph.rtf $(DOCDIR)/wxgraph.hpj
        cd $(DOCDIR)
        -erase wxgraph.ph
        hc wxgraph
        cd $(SRCDIR)

$(DOCDIR)/wxgraph.rtf:         $(DOCDIR)/manual.tex $(DOCDIR)/classes.tex
        cd $(DOCDIR)
        -wx tex2rtf $(DOCDIR)\manual.tex $(DOCDIR)\wxgraph.rtf -twice -winhelp
        cd $(SRCDIR)

$(DOCDIR)/wxgraph.xlp:         $(DOCDIR)/manual.tex $(DOCDIR)/classes.tex
        cd $(DOCDIR)
        -wx tex2rtf $(DOCDIR)\manual.tex $(DOCDIR)\wxgraph.xlp -twice -xlp
        cd $(SRCDIR)

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws
	-del $(GRAPHLIB)

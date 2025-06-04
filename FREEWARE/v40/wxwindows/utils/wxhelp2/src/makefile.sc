# Symantec C++ makefile for wxHelp
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
!include $(WXDIR)\src\makesc.env

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

HYPDIR = $(WXDIR)\utils\hytext
HYPINC = $(HYPDIR)\src
HYPLIB = $(HYPDIR)\lib\hytext.lib

WXHELPDIR = $(WXDIR)\utils\wxhelp
DOCDIR = $(WXHELPDIR)\docs

CC=sc
RC=rc
CFLAGS = -o -ml -W -Dwx_msw
LDFLAGS = -ml -W

INCLUDE=$(BASEINC);$(MSWINC);$(HYPINC)

LIBS=$(WXLIB) $(HYPLIB) libw.lib commdlg.lib ddeml.lib shell.lib

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.rc.res:
	*$(RC) -r -I$(INCLUDE) $<

wxhelp.exe: wxhelp.obj wxhelp.def wxhelp.res
	*$(CC) $(LDFLAGS) -o$@ wxhelp.obj wxhelp.def $(LIBS)
	*$(RC) -k wxhelp.res

wxhelp.obj: wxhelp.$(SRCSUFF) wxhelp.h $(HYPINC)\hytext.h

# Tests out calling wxhelp.exe from an application
test.exe: test.obj test.def test.res
	*$(CC) $(LDFLAGS) -o$@ test.obj test.def $(LIBS)
	*$(RC) -k test.res

# No PostScript production on the PC: just check it goes through Latex
docs:	dvi hlp xlp htm
dvi:	$(WXHELPDIR)/docs/manual.dvi
hlp:	$(WXHELPDIR)/docs/wxhelp.hlp
rtf:	$(WXHELPDIR)/docs/wxhelp.rtf
xlp:    $(WXHELPDIR)/docs/wxhelp.xlp
html:    $(WXHELPDIR)/docs/wxhelp.html

$(WXHELPDIR)/docs/manual.dvi:	$(WXHELPDIR)/docs/manual.tex
	cd $(WXHELPDIR)/docs
        latex manual
        latex manual
        makeindx manual
        latex manual
#        dvips -f -r < manual.dvi > manual.ps

$(WXHELPDIR)/docs/wxhelp.rtf:       $(DOCDIR)/manual.tex
        cd $(DOCDIR)
        -wx tex2rtf $(DOCDIR)\manual.tex $(DOCDIR)\wxhelp.rtf -twice $(WINHELP)
        cd $(WXHELPDIR)/src

$(WXHELPDIR)/docs/wxhelp.hlp:       $(DOCDIR)/wxhelp.rtf
        cd $(DOCDIR)
        hc wxhelp
        cd $(WXHELPDIR)/src

$(WXHELPDIR)/docs/wxhelp.xlp:         $(DOCDIR)/manual.tex
        cd $(DOCDIR)
        -wx tex2rtf $(DOCDIR)\manual.tex $(DOCDIR)\wxhelp.xlp -xlp -twice
        cd $(WXHELPDIR)/src

$(WXHELPDIR)/docs/wxhelp.html:         $(DOCDIR)/manual.tex
        cd $(DOCDIR)
        -wx tex2rtf $(DOCDIR)\manual.tex $(DOCDIR)\wxhelp.htm -html -twice
        cd $(WXHELPDIR)/src

clean:
        -del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws

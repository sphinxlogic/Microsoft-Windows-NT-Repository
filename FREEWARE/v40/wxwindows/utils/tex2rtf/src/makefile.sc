# Symantec C++ makefile for the tex2rtf utility
# NOTE that peripheral libraries are now dealt in main wxWindows makefile.

WXDIR = $(WXWIN)
!include $(WXDIR)\src\makesc.env

WXLIB = $(WXDIR)\lib\wx.lib
INCDIR = $(WXDIR)\include
MSWINC = $(INCDIR)\msw
BASEINC = $(INCDIR)\base

TEX2RTFDIR = $(WXDIR)\utils\tex2rtf
WXHELPDIR = $(WXDIR)\utils\wxhelp
WXHELPINC = $(WXHELPDIR)\src
TEX2ANYLIB = $(TEX2RTFDIR)\lib\tex2any.lib

DOCDIR = $(TEX2RTFDIR)\docs
SRCDIR = $(TEX2RTFDIR)\src

# Default is to output RTF for WinHelp
WINHELP=-winhelp

CC=sc
RC=rc
CFLAGS = -o -ml -W -Dwx_msw
LDFLAGS = -ml -W

INCLUDE=$(BASEINC);$(MSWINC);$(WXHELPINC)

LIBS=$(WXLIB) $(TEX2ANYLIB) libw.lib commdlg.lib ddeml.lib shell.lib

OBJS = tex2rtf.obj rtfutils.obj xlputils.obj htmlutil.obj readshg.obj table.obj

.$(SRCSUFF).obj:
        *$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

.rc.res:
        *$(RC) -r -I$(INCLUDE) $<

tex2rtf.exe: $(OBJS) tex2rtf.def tex2rtf.res $(TEX2ANYLIB)
#       *$(CC) $(LDFLAGS) -o$@ $(OBJS) tex2rtf.def $(LIBS)
        link @tex2rtf.rsp
        *$(RC) -k tex2rtf.res

$(TEX2ANYLIB): tex2any.obj texutils.obj
        -del $(TEX2ANYLIB)
        *lib $(TEX2ANYLIB) y tex2any.obj texutils.obj, nul;

tex2any.obj: tex2any.$(SRCSUFF) tex2any.h

texutils.obj: texutils.$(SRCSUFF) tex2any.h

tex2rtf.obj: tex2rtf.$(SRCSUFF) bmputils.h tex2rtf.h tex2any.h

rtfutils.obj: rtfutils.$(SRCSUFF) tex2rtf.h bmputils.h tex2any.h table.h

xlputils.obj: xlputils.$(SRCSUFF) tex2rtf.h rtfutils.h tex2any.h

htmlutil.obj: htmlutil.$(SRCSUFF) tex2rtf.h tex2any.h table.h

readshg.obj: readshg.$(SRCSUFF)

table.obj: table.$(SRCSUFF) table.h tex2any.h

# No PostScript production on the PC: just check it goes through Latex
docs:   dvi hlp xlp html
dvi:    $(DOCDIR)\tex2rtf.dvi
hlp:    $(DOCDIR)\tex2rtf.hlp
rtf:    $(DOCDIR)\tex2rtf.rtf
xlp:    $(DOCDIR)\tex2rtf.xlp
html:   $(DOCDIR)\tex2rtf.html

$(DOCDIR)/tex2rtf.dvi:  $(DOCDIR)/tex2rtf.tex
	cd $(DOCDIR)
	latex tex2rtf
	latex tex2rtf
	makeindx tex2rtf
	latex tex2rtf
#        dvips -f -r < tex2rtf.dvi > tex2rtf.ps

$(DOCDIR)/tex2rtf.rtf:
	cd $(DOCDIR)
	-wx tex2rtf $(DOCDIR)\tex2rtf.tex $(DOCDIR)\tex2rtf.rtf -twice $(WINHELP)
	cd $(SRCDIR)

$(DOCDIR)/tex2rtf.xlp:         $(DOCDIR)/tex2rtf.tex
	cd $(DOCDIR)
	-wx tex2rtf $(DOCDIR)\tex2rtf.tex $(DOCDIR)\tex2rtf.xlp -xlp -twice
	cd $(SRCDIR)

$(DOCDIR)/tex2rtf.html:         $(DOCDIR)/tex2rtf.tex
	cd $(DOCDIR)
	-wx tex2rtf $(DOCDIR)\tex2rtf.tex $(DOCDIR)\tex2rtf.htm -html -twice
	cd $(SRCDIR)

test.rtf:   test.tex
	-wx $(TEX2RTFDIR)\src\tex2rtf $(TEX2RTFDIR)\src\test.tex $(TEX2RTFDIR)\src\test.rtf $(WINHELP)

clean:
	-del *.obj
	-del *.exe
	-del *.res
	-del *.map
	-del *.rws
	-del $(TEX2ANYLIB)

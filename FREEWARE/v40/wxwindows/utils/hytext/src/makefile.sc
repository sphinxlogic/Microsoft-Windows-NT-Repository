# Symantec C++ makefile for the hypertext library
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

DOCDIR = $(HYPDIR)\docs
SRCDIR = $(HYPDIR)\src

# Default is to output RTF for WinHelp
WINHELP=-winhelp

CC=sc
CFLAGS = -o -ml -W -Dwx_msw

INCLUDE=$(BASEINC);$(MSWINC)

OBJS = hytext.obj

.$(SRCSUFF).obj:
	*$(CC) -c $(CFLAGS) -I$(INCLUDE) $<

$(HYPLIB): $(OBJS)
 	-del $(HYPLIB)
	*lib $(HYPLIB) y $(OBJS), nul;

$(OBJS): $(HYPINC)\hytext.h

# Making documents
docs:   hlp xlp htm
htm:    $(DOCDIR)/hytext.htm
xlp:    $(DOCDIR)/hytext.xlp
rtf:    $(DOCDIR)/hytext.rtf
hlp:    $(DOCDIR)/hytext.hlp

$(DOCDIR)/hytext.hlp:         $(DOCDIR)/hytext.rtf $(DOCDIR)/hytext.hpj
        cd $(DOCDIR)
        -erase hytext.ph
        hc hytext
        cd $(SRCDIR)

$(DOCDIR)/hytext.rtf:         $(DOCDIR)/classes.tex $(DOCDIR)/manual.tex
        cd $(DOCDIR)
        -wx tex2rtf $(DOCDIR)\manual.tex $(DOCDIR)\hytext.rtf -twice $(WINHELP)
        cd $(SRCDIR)

$(DOCDIR)/hytext.xlp:         $(DOCDIR)/manual.tex $(DOCDIR)/classes.tex
        cd $(DOCDIR)
        -wx tex2rtf $(DOCDIR)\manual.tex $(DOCDIR)\hytext.xlp -twice -xlp
        cd $(SRCDIR)

$(DOCDIR)/hytext.html:         $(DOCDIR)/manual.tex $(DOCDIR)/classes.tex
        cd $(DOCDIR)
        -wx tex2rtf $(DOCDIR)\manual.tex $(DOCDIR)\hytext.htm -twice -html
        cd $(SRCDIR)

clean:
        -del *.obj
	-del $(HYPLIB)

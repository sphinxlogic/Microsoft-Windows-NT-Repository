#
# File:		makefile.wat
# Author:	Julian Smart
# Created:	1996
# Updated:
# Copyright:	(c) 1996, AIAI, University of Edinburgh
#
# "%W% %G%"
#
# Makefile : Builds wxWindows samples for MS Windows, Watcom C++ (32-bit).

WXDIR = ..

!include $(WXDIR)\src\makewat.env

THISDIR = $(WXDIR)\samples

# All except those which may need recompilation with different wx_setup.h settings
all:	hello ipc forty form animate mdi minimal fractal panel layout &
          printing toolbar buttnbar docview dialogs types wxpoem pressup bombs splitter # vlist memcheck odbc static

clean:	hello_clean ipc_clean forty_clean form_clean animate_clean mdi_clean minimal_clean fractal_clean panel_clean layout_clean &
          printing_clean toolbar_clean buttnbar_clean docview_clean dialogs_clean types_clean wxpoem_clean pressup_clean bombs_clean splitter_clean
           # vlist_clean memcheck_clean odbc_clean static_clean

# SAMPLES
hello:	.SYMBOLIC
	cd $(WXDIR)\samples\hello
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

hello_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\hello
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

ipc:	.SYMBOLIC
	cd $(WXDIR)\samples\ipc
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

ipc_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\ipc
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

form:	.SYMBOLIC
	cd $(WXDIR)\samples\form
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

form_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\form
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

forty:	.SYMBOLIC
	cd $(WXDIR)\samples\forty
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

forty_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\forty
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

animate:	.SYMBOLIC
	cd $(WXDIR)\samples\animate
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

animate_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\animate
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

mdi:	.SYMBOLIC
	cd $(WXDIR)\samples\mdi
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

mdi_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\mdi
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

minimal:	.SYMBOLIC
	cd $(WXDIR)\samples\minimal
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

minimal_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\minimal
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

splitter:	.SYMBOLIC
	cd $(WXDIR)\samples\splitter
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

splitter_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\splitter
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

fractal:	.SYMBOLIC
	cd $(WXDIR)\samples\fractal
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

fractal_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\fractal
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

vlist:	.SYMBOLIC
	cd $(WXDIR)\samples\vlist
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

vlist_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\vlist
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

panel:	.SYMBOLIC
	cd $(WXDIR)\samples\panel
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

panel_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\panel
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

layout:	.SYMBOLIC
	cd $(WXDIR)\samples\layout
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

layout_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\layout
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

static:	.SYMBOLIC
	cd $(WXDIR)\samples\static
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

static_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\static
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

printing:	.SYMBOLIC
	cd $(WXDIR)\samples\printing
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

printing_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\printing
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

toolbar:	.SYMBOLIC
	cd $(WXDIR)\samples\toolbar
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

toolbar_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\toolbar
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

buttnbar:	.SYMBOLIC
	cd $(WXDIR)\samples\buttnbar
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

buttnbar_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\buttnbar
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

docview:	.SYMBOLIC
	cd $(WXDIR)\samples\docview
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

docview_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\docview
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

memcheck:	.SYMBOLIC
	cd $(WXDIR)\samples\memcheck
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

memcheck_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\memcheck
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

odbc:	.SYMBOLIC
	cd $(WXDIR)\samples\odbc
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

odbc_clean:	.SYMBOLIC
	cd $(WXDIR)\samples\odbc
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" clean
	cd $(THISDIR)

dialogs:        .SYMBOLIC
        cd $(WXDIR)\samples\dialogs
        wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
        cd $(THISDIR)

dialogs_clean:        .SYMBOLIC
        cd $(WXDIR)\samples\dialogs
        wmake -f makefile.wat clean
        cd $(THISDIR)

types:  .SYMBOLIC
        cd $(WXDIR)\samples\types
        wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)"  all
        cd $(THISDIR)

types_clean:  .SYMBOLIC
        cd $(WXDIR)\samples\types
        wmake -f makefile.wat clean
        cd $(THISDIR)

mfc:    .SYMBOLIC
        cd $(WXDIR)\samples\mfc
        wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
        cd $(THISDIR)

mfc_clean:    .SYMBOLIC
        cd $(WXDIR)\samples\mfc
        wmake -f makefile.wat clean
        cd $(THISDIR)

wxpoem:         .SYMBOLIC
        cd $(WXDIR)\samples\wxpoem
        wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
        cd $(THISDIR)

wxpoem_clean:         .SYMBOLIC
        cd $(WXDIR)\samples\wxpoem
        wmake -f makefile.wat clean
        cd $(THISDIR)

pressup:        .SYMBOLIC
        cd $(WXDIR)\samples\pressup
        wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
        cd $(THISDIR)

pressup_clean:        .SYMBOLIC
        cd $(WXDIR)\samples\pressup
        wmake -f makefile.wat clean
        cd $(THISDIR)

bombs:        .SYMBOLIC
        cd $(WXDIR)\samples\bombs
        wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
        cd $(THISDIR)

bombs_clean:        .SYMBOLIC
        cd $(WXDIR)\samples\bombs
        wmake -f makefile.wat clean
        cd $(THISDIR)

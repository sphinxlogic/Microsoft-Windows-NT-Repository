#
# File:		makefile.wat
# Author:	Julian Smart
# Created:	1996
# Updated:
# Copyright:	(c) 1996, AIAI, University of Edinburgh
#
# "%W% %G%"
#
# Makefile : Builds wxWindows utilities for MS Windows, Watcom C++ (32-bit).

WXDIR = ..

!include $(WXDIR)\src\makewat.env

THISDIR=$(WXDIR)\utils

all:	all_utils all_execs

all_utils:	hytext wxtree wxgraph wxgrid wxtab wxprop mfutils # dib rcparser prologio

all_execs:	wxhelp tex2rtf wxbuild colours dialoged #winstall xpmshow

all_demos:	wxtab_demo wxgrid_demo wxtree_demo wxgraph_demo

clean:  hytext_clean wxtree_clean wxgraph_clean wxgrid_clean wxtab_clean wxprop_clean mfutils_clean &
 wxhelp_clean tex2rtf_clean wxbuild_clean colours_clean dialoged_clean

xpmshow:	.SYMBOLIC
	cd $(WXDIR)\utils\xpmshow\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

xpmshow_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\xpmshow\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

hytext:	.SYMBOLIC
	cd $(WXDIR)\utils\hytext\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

hytext_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\hytext\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

wxhelp:	hytext
	cd $(WXDIR)\utils\wxhelp\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

wxhelp_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\wxhelp\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

tex2rtf:	.SYMBOLIC
	cd $(WXDIR)\utils\tex2rtf\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

tex2rtf_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\tex2rtf\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

prologio:	.SYMBOLIC
	cd $(WXDIR)\utils\prologio\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

prologio_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\prologio\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

wxtree:	.SYMBOLIC
	cd $(WXDIR)\utils\wxtree\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

wxtree_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\wxtree\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

wxtree_demo:	.SYMBOLIC
	cd $(WXDIR)\utils\wxtree\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" test.exe
	cd $(THISDIR)

wxgraph:	.SYMBOLIC
	cd $(WXDIR)\utils\wxgraph\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

wxgraph_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\wxgraph\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

wxgraph_demo:	.SYMBOLIC
	cd $(WXDIR)\utils\wxgraph\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" test.exe
	cd $(THISDIR)

wxtab:	.SYMBOLIC
	cd $(WXDIR)\utils\wxtab\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

wxtab_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\wxtab\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

wxtab_demo:	.SYMBOLIC
	cd $(WXDIR)\utils\wxtab\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" test.exe
	cd $(THISDIR)

wxprop:	.SYMBOLIC
	cd $(WXDIR)\utils\wxprop\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

wxprop_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\wxprop\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

wxprop_demo:	.SYMBOLIC
	cd $(WXDIR)\utils\wxprop\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" test.exe
	cd $(THISDIR)

wxgrid:	.SYMBOLIC
	cd $(WXDIR)\utils\wxgrid\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

wxgrid_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\wxgrid\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

wxgrid_demo:	.SYMBOLIC
	cd $(WXDIR)\utils\wxgrid\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" test.exe
	cd $(THISDIR)

wximage:	.SYMBOLIC
	cd $(WXDIR)\utils\wximage\win
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

wximage_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\wximage\win
	wmake -f makefile.wat clean
	cd $(THISDIR)

wxhtml:	.SYMBOLIC
	cd $(WXDIR)\utils\wxweb\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

wxhtml_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\wxweb\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

wxbuild:	.SYMBOLIC
	cd $(WXDIR)\utils\wxbuild\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

wxbuild_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\wxbuild\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

dialoged:	wxprop
	cd $(WXDIR)\utils\dialoged\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

dialoged_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\dialoged\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

rcparser:	.SYMBOLIC
	cd $(WXDIR)\utils\rcparser\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

rcparser_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\rcparser\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

winstall:	.SYMBOLIC
	cd $(WXDIR)\utils\winstall\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

winstall_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\winstall\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

mfutils:	.SYMBOLIC
	cd $(WXDIR)\utils\mfutils\src
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

mfutils_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\mfutils\src
	wmake -f makefile.wat clean
	cd $(THISDIR)

dib:	.SYMBOLIC
	cd $(WXDIR)\utils\dib
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

dib_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\dib
	wmake -f makefile.wat clean
	cd $(THISDIR)

colours:	.SYMBOLIC
	cd $(WXDIR)\utils\colours
	wmake -f makefile.wat FINAL="$(FINAL)" DEBUG="$(DEBUG)" all
	cd $(THISDIR)

colours_clean:	.SYMBOLIC
	cd $(WXDIR)\utils\colours
	wmake -f makefile.wat clean
	cd $(THISDIR)

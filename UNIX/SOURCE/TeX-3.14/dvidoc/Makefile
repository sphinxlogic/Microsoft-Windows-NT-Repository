#-----------------------------------------------------------------------
# Usage:
#
#	make dviman
# or
#	make dvidriver
# or
#	make clean
#
# [17-Apr-87]
#-----------------------------------------------------------------------

LATEX = latex 

CP = cp

.SUFFIXES:

.SUFFIXES:	.dvi .ltx .ids .idx

.idx.ids:
	$(CP) $*.idx $*.id
	texidx $*.id
	$(RM) $*.id.*

all:	dviman dvidriver

dviman:	dviman.ltx
	$(LATEX) dviman.ltx

dvidriver:	dvidriver.ltx dvidriver.sty
	$(LATEX) dvidriver.ltx
	make dvidriver.ids
	$(LATEX) dvidriver.ltx
	make dvidriver.ids

dvidriver.ids:	dvidriver.idx

clean:
	$(RM) *.dvi
	$(RM) *.lst
	$(RM) *.aux


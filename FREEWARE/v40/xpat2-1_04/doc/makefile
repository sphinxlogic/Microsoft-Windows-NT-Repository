all: xpat2.dvi

xpat2.ps: xpat2.dvi
	dvips -f < xpat2.dvi > xpat2.ps

xpat2.dvi: xpat2.tex
	latex xpat2
	latex xpat2
	latex xpat2

clean:
	rm -f *~ *.log *.aux *.toc *.dvi *.ps

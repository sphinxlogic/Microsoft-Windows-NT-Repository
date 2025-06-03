# This file is for GRASS, a geographic information system. 
# To compile, make modifications below (if necessary) then
# % ln -s makefile.g Gmakefile
# % gmake4.0
#
# NOTE: this creates a binary called 'g.gnuplot' and is located in
#       $GISBASE/bin.  
#       A help file is installed in $(GISBASE)/man/helpp/gnuplot.gih
#
# GRASS driver written by:
# James Darrell McCauley          Department of Ag Engr, Purdue Univ
# mccauley@ecn.purdue.edu         West Lafayette, Indiana 47907-1146
#
# Last modified: 01 Mar 1993
#
# Known Bugs:  There may be a problem with fifo's. Then again, there may not.
#              Drawing non-filled point types is slow.
#
# Things to do: modify text function to change fonts? will make g.gnuplot
#               input files incompatible with gnuplot
#
# Modification History:
# <15 Jun 1992>	First version created with GNUPLOT 3.2
# <15 Feb 1993> Modified to work with frames
# <16 Feb 1993> Added point types triangle (filled and unfilled), 
#               inverted-triangle (filled and unfilled), 
#               circle (filled and unfilled), and filled box.
#               Graph is no longer erased after g.gnuplot is finished.
# <01 Mar 1993> Modified to work with 3.3b9
#
#############################################################################
# Where to send email about bugs and comments 
EMAIL=grassp-list@moon.cecer.army.mil

HELPDEST=$(GISBASE)/man/helpp/gnuplot.gih
#HELPDEST=docs/gnuplot.gih
################### Don't touch anything below this line ###################
GTERMFLAGS = -DGRASS 

EXTRA_CFLAGS=-DREADLINE -DNOCWDRC $(GTERMFLAGS) \
	-DCONTACT=\"$(EMAIL)\" -DHELPDEST=\"$(HELPDEST)\"

OFILES = \
	binary.o \
	bitmap.o \
	command.o \
	contour.o \
	eval.o \
	gnubin.o \
	graph3d.o \
	graphics.o \
	help.o \
	internal.o \
	misc.o \
	parse.o \
	plot.o \
	readline.o \
	scanner.o \
	setshow.o \
	specfun.o \
	standard.o \
	term.o \
	util.o \
	version.o 

#$(BIN_MAIN_CMD)/g.gnuplot: $(OFILES) $(DISPLAYLIB) $(RASTERLIB) $(GISLIB) 
g.gnuplot: $(OFILES) $(DISPLAYLIB) $(RASTERLIB) $(GISLIB) 
	$(CC) $(LDFLAGS) -o $@ $(OFILES) $(DISPLAYLIB) $(RASTERLIB) $(GISLIB) $(TERMLIB) $(MATHLIB)

$(HELP): $(GISBASE)/man/helpp/g.gnuplot.gih
	( cd docs; $(MAKE) -DHELPDEST="$(HELPDEST)" gnuplot.gih )
	 mkdir $(GISBASE)/man/helpp
	 cp gnuplot.gih $(GISBASE)/man/helpp/g.gnuplot.gih

# Dependencies

#term.o: term.h term.c term/grass.trm
term.o: term.h term.c 

$(OFILES): plot.h

command.o help.o misc.o: help.h

command.o graphics.o graph3d.o misc.o plot.o setshow.o term.o: setshow.h

bitmap.o term.o: bitmap.h

################################################################
$(RASTERLIB): #
$(DISPLAYLIB): #
$(GISLIB): #



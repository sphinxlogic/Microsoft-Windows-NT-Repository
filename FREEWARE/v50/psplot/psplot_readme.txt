Here are the files relating to the PSPLOT library:

grman.doc	   MS Word version of psplot documentation
grman.ps	   PostScript version of psplot documentation
grmana4.doc	   MS Word version of psplot documentation for A4 paper
grmana4.ps	   PostScript version of psplot documentation for A4 paper
grmaneb.ps	   PostScript version of psplot documentation, even pages
		   printed in reverse order.
grmano.ps	   PostScript version of psplot documentation, odd pages.
npzdmain.for	   Example Fortran plotting program
		   Note: This is intended as an example program, not an
		         program to be executed.  
npzdmain.ps	   Output PostScript file created with npzdmain.for (1.2Mb!!)
onehalf3d.for	   Fortran plotting program used to create onehalf3d.ps
onehalf3d.ps	   Output PostScript file created with onehalf3d.for
pslayout.com	   VMS script for running pslayout.for
pslayout.for	   Utility for printing multiple frames/page of a PostScript 
                   file	created with psplot
pspage.com	   VMS script for running pspage.for
pspage.for	   Utility for printing certain pages of a PostScript file
		   created with psplot
psplot.txt	   Source code for the psplot library.
psplot_readme.txt  This file
release_notes.txt  Notes and changes relating to PSPLOT

The documentation is contained in grman.ps. If you'd prefer to
print the documentation double-side, print grmaneb.ps, flip the pages,
and print grmano.ps.

The source code (psplot.txt) is standard Fortran 77.  You can either 
compile the whole thing at once, or separate the individual routines, 
compile separately, and place in a library.  

The second method is best if you plan on making periodic changes to 
the code, in that you don't have to recompile everything all the time.
The routines are delimited in the .txt file with C*****routinename. 
You can write a short script to extract the individual files out 
if you don't want to do it by hand.

Any questions, bug encounters, suggestions should be addressed to:

Kevin E. Kohler   <kevin@ocean.nova.edu>   
Senior Programmer/Coordinator of Computing Services
Nova Southeastern University Oceanographic Center  
8000 North Ocean Drive  Dania, Florida  33004      
Ph:  954.920.1909    Fax: 954.921.7764

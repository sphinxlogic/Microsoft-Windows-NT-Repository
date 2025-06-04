$! pslayout
$! This procedure prints selected pages from a postscript file
$! created with the psplot plotting library. It allows the user to
$! specify number of frames per page.
$!
$ say:="write sys$output"
$ if p1 .eqs. "" .or. p2 .eqs. "" .or. p4 .eqs. "" .or. p5 .eqs. "" -
                 .or. p6 .eqs. "" .or. p7 .eqs. "" 
$ then
$      say " "
$      say "        PSLAYOUT requires 8 input arguments, separated by spaces:"
$      say " "
$      say "        Arg. 1: Input psplot library PS file."
$      say " "
$      say "        Arg. 2: Pages to print (e.g. 1,3,5,7-11,18)."
$      say "                No spaces are allowed in pages argument."
$      say "                Pages must be listed in ascending order."
$      say " "
$      say "                Enter A or ALL for all pages."
$      say " "
$      say "                To print pages with a constant increment,
$      say "                use the form xx,yy,Szz where xx is the first
$      say "                page, yy is the last page (can be "A" or "ALL"),
$      say "                and zz is the increment.
$      say " "
$      say "        Arg. 3: Name of output file 
$      say "                ("""" if you don't want a permanent output file)"
$      say " "
$      say "        Arg. 4: Immediate print (T or F)
$      say " "
$      say "        Arg. 5: Upper or lower paper tray (U or L)"
$      say " "
$      say "        Arg. 6: Number of frames per row/frames per column"
$      say " "
$      say "        Arg. 7: Portrait mode (T or F)"
$      say " "
$      say "        Arg. 8: Frame locations to skip (first,last,inc)
$      say " "
$      exit
$ endif
$ ifile=p1
$ pages=p2
$ ofile=p3
$ printnow=p4
$ ptray=p5
$ irowicol=p6
$ portrait=p7
$ pskip=p8
$ run pslayout
$ exit

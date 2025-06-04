$! vtkwild.com - iterate a command over a wild-card file list
$
$!*****************************************************************************
$! VToolkit (1995) 	"If it doesn't use VToolkit, it isn't cool!"	      *
$! (c) Steve Klein							      *
$! Unpublished rights reserved under the copyright laws of the United States. *
$!*****************************************************************************
$
$ if p1 .eqs. "" then inquire p1 "Command"
$ if p1 .eqs. "" then exit
$ if p2 .eqs. "" then inquire p2 "File names"
$ if p2 .eqs. "" then exit
$flush_loop:		! reset wild-card context
$ x = f$search("foobarf")
$ if "''x'" .nes. "" then goto flush_loop
$loop:			! loop through matching files
$ x = f$search(p2)
$ if "''x'" .eqs. "" then exit
$ write sys$output "$''p1' ''x' ''p3' ''p4' ''p5' ''p6' ''p7'"
$ on severe then continue
$ define/user sys$input tt:
$ 'p1 'x 'p3 'p4 'p5 'p6 'p7
$ on severe then exit $status
$ goto loop

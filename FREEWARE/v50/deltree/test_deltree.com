$ !
$ ! P1 = "TEST" invoqued from MMS/MMK
$ ! P1 = "CREATE" creates directory levels only. May be used fro /DETACH tests
$ !      not yet implemented in this primitive test script.
$ 
$ echo := write sys$output
$
$ if P1.eqs.""
$ then	echo "Use MM{S|K} TEST"
$ 	exit 44
$ endif
$
$ cdir	= f$environment("PROCEDURE")
$ rdir	= f$parse(cdir,,,,"NO_CONCEAL")-"]["
$ rdir	= F$EXTRACT(0,F$LOCATE("]",rdir),rdir)
$ cdir	= f$element(0,"]",cdir-"][")+"]"
$ 
$ define/trans=conceal TEST_ROOT  'rdir'.]
$ set default TEST_ROOT:[000000]
$ 
$ if f$search("TEST.DIR").eqs.""
$ then
$ 	echo "Creating sub-directories"
$ 	CREATE/DIR/LOG [.TEST]
$ 	CREATE/DIR/LOG [.TEST.BLOP]
$ 	CREATE/DIR/LOG [.TEST.BLIP]
$ 	CREATE/DIR/LOG [.TEST.BLIP.BLUP]
$ 	CREATE/DIR/LOG [.TEST.BLIP.BLUP.BLAP]
$ 	CREATE/DIR/LOG [.TEST.BLIP.BLUP.BLAP.BLEP]
$ 	i = 0
$ 	echo "Creating 100 files in [.TEST]"
$ 	LOOP:
$ 		create [.TEST]TEST.'i'
$ 		i = i + 1
$ 	if i.lt.100 then goto LOOP
$
$ 	echo "Adding a trap file .BLOP"
$
$ 	! Because "other" deltree programs fails to delete those
$ 	create [.TEST].BLOP
$ 	create [.TEST.BLIP.BLUP.BLAP.BLEP]LEVEL5.;
$ else
$ 	echo "TEST.DIR exists. Assuming that files are present"
$ endif
$ if f$search("TEMP2.DIR").eqs.""
$ then
$ 	CREATE/DIR/LOG [.TEMP2]
$ 	CREATE/DIR/LOG [.TEMP2.BLOP]
$ 	echo "Duplicating [.TEST] in [.TEMP2.BLOP]"
$ 	BACKUP TEST_ROOT:[TEST...]*.* TEST_ROOT:[TEMP2.BLOP...]*
$ else
$ 	echo "TEMP2.DIR exists. Assuming that files are present"
$ endif
$ if f$search("TEMP1.DIR").eqs.""
$ then
$ 	CREATE/DIR/LOG [.TEMP1]
$ else
$ 	echo "TEMP1.DIR exists. Assuming that files are present"
$ endif
$
$
$ set default 'cdir'
$ 
$ if P1.eqs."CREATE" then exit
$ 
$ DELTREE := $'cdir'DELTREE.'P1'
$ echo "Deleting directory TEMP1 TEMP2 and TEST"
$ echo "  - 100 files + 2 tricky + 5 subdir + 1 top"
$ echo "  - 1 relative top ([.XXX.YYY] syntax) with all preceding files"
$ echo "  - 1 top (syntax [.XXX])"
$
$ on error then exit 44
$ show sym deltree
$ DELTREE/ALL/NOCONF/LOG/DEBUG TEST.DIR,TEMP1.DIR,[.TEMP2.BLOP],[.TEMP2]
$
$ if f$search("TEST.DIR").eqs.""
$ then	DELTREE/VERSION
$ 	echo "DELTREE seems to be OK using ''P1'"
$ else	! If this happen, better send me a problem report.
$ 	echo "DELTREE failed (?)"
$ 	exit 44
$ endif
$
$ exit

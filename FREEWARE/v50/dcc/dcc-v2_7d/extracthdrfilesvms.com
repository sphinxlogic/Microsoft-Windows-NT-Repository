$ ! EXTRACTHDRFILESVMS.COM
$ ! Author: Jerome LAURET <JLAURET@mail.chem.sunysb.edu>
$ echo := write sys$output
$ echo "--------------------------------------------------------------------"
$ echo "This procedure will extract all C header files and place them in the"
$ echo "DCCSYSHDRDIR: directory."
$ echo "--------------------------------------------------------------------"
$ echo ""
$ if f$trnlnm("DCCSYSHDRDIR").eqs.""
$  then	! Impossible to continue
$ 	echo "The logical DCCSYSHDRDIR is undefined. Abort !!"
$ 	exit 44
$ endif
$ ! Else
$ echo "Warning: executing this  procedure requires some system  privileges,"
$ echo "unless you redefine the logical DCCSYSHDRDIR to point to a directory"
$ echo "you own. Proceed ? (if no, you can always execute EXTRACTHDRFILESVMS"
$ inquire ANS ".COM later) ([y]/n)"
$ ans = f$extract(0,1,ans)
$ if ans.eqs."N" then exit
$ on error then goto create_dir
$ open/write filout DCCSYSHDRDIR:test.lis
$ close filout
$ goto extract_all
$create_dir:
$ create/dir/protection=(W:re) DCCSYSHDRDIR
$extract_all:
$ echo ""
$ call ExtractHeader DECC$RTLDEF.TLB
$ call ExtractHeader SYS$STARLET_C.TLB
$ call ExtractHeader SYS$LIB_C.TLB
$ echo " -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"
$ echo ""
$ echo "Cleaning ..."
$ delete/nolog/noconfirm DCC_H.LIS;*
$ set file/protection=(w:r) DCCSYSHDRDIR:*.h;
$ purge/nolog/keep=1 DCCSYSHDRDIR:*.h
$ echo ""
$ echo "All done ..."
$ echo ""
$exit
$!
$ExtractHeader: subroutine
$ echo " -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"
$ echo " Extracting headers from ''P1'"
$ if f$search("SYS$LIBRARY:''P1'").nes.""
$  then	! Go ahead
$	LIBRARY/LIST=DCC_H.LIS SYS$LIBRARY:'P1'
$	open/read inFile DCC_H.LIS
$	on error then goto ELOOP_H
$	i = 0
$LOOP_H:
$	read/end=ELOOP_H inFile ligne
$	if f$element(1," ",ligne).eqs." " .and. ligne.nes.""
$	 then	! This is a module reference"
$		!echo "	Header file ''ligne'"
$		library/extract='ligne' SYS$LIBRARY:'P1' /OUT=DCCSYSHDRDIR:'ligne'.h
$		i = i + 1
$	endif
$	goto LOOP_H
$ELOOP_H:
$	close inFile
$	echo "	''i' modules extracted from ''P1'"
$  else echo "Could not find SYS$LIBRARY:''P1' !!!"
$ endif
$ endSubroutine
$ ! End EXTRACTHDRFILESVMS.COM

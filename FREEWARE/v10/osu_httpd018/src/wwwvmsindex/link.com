$ v = 'f$verify(0)'
$! LINK.COM
$!      link VMS Gopher Server Indexing utilites
$!
$!  19940423	F.Macrides		macrides@sci.wfeb.edu
$!		temporary mods for field test release - keep executables
$!		in the sources directory.
$!  19931120    F.Macrides		macrides@sci.wfeb.edu
$!		formatting fixups
$!  19931112	D.Sherman		dennis_sherman@unc.edu
$!		rewritten for Gopher1_2VMS-1 release
$!  19930909    D.Sherman               dennis_sherman@unc.edu
$!              rewritten version for Gopher1_2VMS0, based in part on
$!              the Gopher1_2VMS0 client make.com
$!
$ ON CONTROL_Y THEN GOTO CLEANUP
$ ON ERROR THEN GOTO CLEANUP
$!
$ if f$trnlnm("VAXCMSG") .eqs. "DECC$MSG" .or. -
     f$trnlnm("DECC$CC_DEFAULT") .eqs. "/DECC"
$ then
$!      DECC/Alpha:
$   optfile = "DECC"
$ else
$!      VAXC:
$   optfile = "VAXC"
$ endif
$ show sym optfile
$!
$ v1 = f$verify(1)
$ link /executable=[]build_index.exe -
    []build_index.obj,-
    []indexcld.obj,-
    []ident/opt,-
    []'optfile'/opt
$!
$ link /executable=[]query.exe -
    []query.obj,-
    []ident/opt,-
    []'optfile'/opt
$!
$ link /executable=[]wwwquery.exe -
    []wwwquery.obj,-
    []ident/opt,-
    []'optfile'/opt
$!
$ v1 = 'f$verify(0)'
$ CLEANUP:
$    v1 = f$verify(v)
$exit

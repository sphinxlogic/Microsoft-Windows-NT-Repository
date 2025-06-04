$ v = 'f$verify(0)'
$! COMPILE.COM
$!	compile VMS Gopher ACL-resident LOOKASIDE utility
$!
$!  19940120	D.Sherman		dennis_sherman@unc.edu
$!		Fixed typo that affected DECC with UCX
$!  19931209    F.Macrides		macrides@sci.wfeb.edu
$!		Fixed cc's for UCX and globalext's when DECC
$!  19931125    F.Macrides		macrides@sci.wfeb.edu
$!		eliminated prefix qualifier
$!  19931120    F.Macrides		macrides@sci.wfeb.edu
$!		formatting and qualifier fixups
$!		don't recompile compatible.c if [-.obj]compatible.obj exists
$!  19931119	D.Sherman		dennis_sherman@unc.edu
$!		rewritten for Gopher1_2VMS-1 release
$!  19930909	D.Sherman		dennis_sherman@unc.edu
$!		rewritten version for Gopher1_2VMS0, based in part on
$!		the Gopher1_2VMS0 client make.com
$!
$ ON CONTROL_Y THEN GOTO CLEANUP
$ ON ERROR THEN GOTO CLEANUP
$!
$ if f$trnlnm("VAXCMSG") .eqs. "DECC$MSG" .or. -
	  f$trnlnm("DECC$CC_DEFAULT") .eqs."/DECC"
$ then
$! DECC:
$    write sys$output "Using DECC:"
$    if f$getsyi("CPU") .ge. 128
$    then ! DECC/AXP
$        v1 = f$verify(1)
$        cc/object=[-.obj]-
           /include=([-],[-.gopherd])-
           /warning=(disable=(implicitfunc,globalext))-
           /tie-
	   /define=(SERVER)
         lookaside.c
$!
$        v1 = 'f$verify(0)'
$    else ! DECC/VAX
$        v1 = f$verify(1)
$        cc/object=[-.obj]-
           /include=([-],[-.gopherd])-
           /warning=(disable=(implicitfunc,globalext))-
	   /define=(SERVER)
         lookaside.c
$!
$        v1 = 'f$verify(0)'
$    endif
$ else
$! VAXC:
$    cc := cc/object=[-.obj]-
             /include=([-],[-.GOPHERD])-
	     /define=(SERVER)
$    write sys$output "Using VAXC:"
$    show sym cc
$    v1 = f$verify(1)
$    cc lookaside.c
$!
$    v1 = 'f$verify(0)'
$ ENDIF
$!
$ CLEANUP:
$    v1 = f$verify(v)
$exit

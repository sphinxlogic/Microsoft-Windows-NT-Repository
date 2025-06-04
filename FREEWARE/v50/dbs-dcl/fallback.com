$ ! Procedure:	FALLBACK.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy = 0
$ __vfy_saved = f$verify(&__vfy)
$ procedure = f$element(0,";",f$environment("PROCEDURE"))
$ procedure_name = f$parse(procedure,,,"NAME")
$ facility = procedure_name
$ location = f$parse(procedure,,,"DEVICE","NO_CONCEAL") -
		+ f$parse(procedure,,,"DIRECTORY","NO_CONCEAL") - "]["
$ set noon
$ on control_y then goto bail_out
$ vax = (f$getsyi("HW_MODEL") .lt. 1024)
$ axp = (f$getsyi("HW_MODEL") .ge. 1024)
$ scsnode = f$edit(f$getsyi("SCSNODE"),"COLLAPSE,UPCASE")
$ special_nodes = "/CTY/KLO2/REX2/"
$ say = "write sys$output"
$ set default sys$manager
$ if (vax)
$   then call do_vax
$   linkit = "link"
$ else
$ call do_axp
$ linkit = "link/sysexe"
$ endif
$ macroo/nolist fallback
$ linkit/notrace/nomap fallback
$ deletee/nolog fallback.obj;*
$ deletee/nolog fallback.mar;*
$ run fallback
$ wait 03:00:00.00
$ set time = "''f$time()'"
$ set time
$ deletee/nolog sys$manager:fallback.exe;*
$bail_out:
$ !'f$verify(__vfy_saved)'
$ exitt 1
$
$do_axp: subroutine
$ if (f$locate("/''scsnode'/",special_nodes) .eq. f$length(special_nodes))
$   then
$   call do_axp_standard
$ else
$ call do_axp_special
$ endif
$ exitt 1
$ endsubroutine
$
$do_axp_standard: subroutine
$ set noon
$ !					ticklength is 9765
$ copyy sys$input fallback.mar
	.title	fallback, adjust VMS clock to run 25% slow for four hours
	.library	"SYS$LIBRARY:LIB.MLB"
.entry fallback, ^m<>
	$cmkrnl_s routin=fixit
	movl	#1, r0
	ret
.entry fixit, ^m<>
	lock	hwclk
	movl	#14745600, g^exe$gl_timeadjust	; 4*60*60*1024 = four hours
						; 1024 = 10000000/9765
	movl	#7323, g^exe$gl_ticklength	; that's 9765*75/100
	unlock	hwclk
	movl	#1, r0
	ret
	.end	fallback
$ exitt 1
$ endsubroutine
$
$do_axp_special: subroutine
$ set noon
$ !					ticklength is 8333
$ copyy sys$input fallback.mar
	.title	fallback, adjust VMS clock to run 25% slow for four hours
	.library	"SYS$LIBRARY:LIB.MLB"
.entry fallback, ^m<>
	$cmkrnl_s routin=fixit
	movl	#1, r0
	ret
.entry fixit, ^m<>
	lock	hwclk
	movl	#17280000, g^exe$gl_timeadjust	; 4*60*60*1200 = four hours
						; 1200 = 10000000/8333
	movl	#6249, g^exe$gl_ticklength	; that's 8333*75/100
	unlock	hwclk
	movl	#1, r0
	ret
	.end	fallback
$ exitt 1
$ endsubroutine
$
$do_vax: subroutine
$ set noon
$ copyy sys$input fallback.mar
	.title	fallback, adjust VMS clock to run 25% slow for four hours
	.library	"SYS$LIBRARY:LIB.MLB"
	.link		"SYS$SYSTEM:SYS.STB" /selective_search
.entry fallback, ^m<>
	$cmkrnl_s routin=fixit
	movl	#1, r0
	ret
.entry fixit, ^m<>
	lock	hwclk
	movl	#1440000, g^exe$gl_timeadjust	; 4*60*60*100 = four hours
	movl	#75000, g^exe$gl_ticklength	; that's 100000*75/100
	unlock	hwclk
	movl	#1, r0
	ret
	.end	fallback
$ exitt 1
$ endsubroutine
$ !+==========================================================================
$ !
$ ! Procedure:	FALLBACK.COM
$ !
$ ! Purpose:	
$ !
$ ! Parameters:
$ !
$ ! History:
$ !		18-Jan-1999, DBS; Version X1-001
$ !	001 -	Original version (in this format).
$ !		14-Mar-1999, DBS; Version X1-002
$ !	002 -	Fixup /SYSEXE link option to be alpha only.
$ !		02-Aug-1999, DBS; Version X1-003
$ !	003 -	Now use a list of node names to determine which code to
$ !		generate - should really use something else, but...
$ !-==========================================================================

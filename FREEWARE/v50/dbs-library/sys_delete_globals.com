$ ! Procedure:	SYS_DELETE_GLOBALS.COM
$ __vfy = "VFY_''f$parse(f$environment("procedure"),,,"name")'"
$ if (f$type('__vfy') .eqs. "") then __vfy_saved = f$verify(0)
$ if (f$type('__vfy') .nes. "") then __vfy_saved = f$verify(&__vfy)
$ disk_checker__ = "dbslibrary:sys_check_sys$disk.com"
$ if (f$search(disk_checker__) .nes. "") then @'disk_checker__'
$ on control_y then goto bail_out
$ on warning then goto no_symbol
$ say = "write sys$output"
$ list_file = "''f$getjpi("", "pid")'_GLOBAL_SYMBOLS.LIST"
$ if (P1 .eqs. "") then goto no_symbol
$ logging = (P2 .eqs. "LOG")
$ definee/nolog sys$error  nla0:
$ definee/nolog sys$output 'list_file'
$ show symbol/global 'P1'
$ deassign sys$output
$ deassign sys$error
$ open/read input 'list_file'
$read_loop:
$   read/end_of_file=end_read_loop input text
$   if (f$locate("==", text) .eq. f$length(text)) then goto read_loop
$   symbol = f$element(0, "=", f$edit(text, "collapse")) - "*"
$   deletee/symbol/global 'symbol'
$   if logging then say " Deleting symbol ''symbol'"
$  goto read_loop
$end_read_loop:
$ close/nolog input
$ goto bail_out
$no_symbol:
$ say "%DELSYM-E-NOSYM, There was nothing to delete"
$ goto bail_out
$bail_out:
$ if (f$search(list_file) .nes. "") then deletee/nolog 'list_file';*
$ !'f$verify(__vfy_saved)'
$ exit
$ !+==========================================================================
$ !
$ ! Procedure:	SYS_DELETE_GLOBALS.COM
$ !
$ ! Purpose:	To delete a set of global symbols.
$ !
$ ! Parameters:
$ !	 P1	The symbol to delete, will usually be a wildcard.
$ !	[P2]	Either LOG or NOLOG (defaults to NOLOG).
$ !
$ ! History:
$ !		12-Oct-1988, DBS; Version V1-001
$ !  	001 -	Original version.
$ !		24-Jan-1989, DBS; Version V1-002
$ !	002 -	Added verify stuff.
$ !		22-Mar-1989, DBS; Version V1-003
$ !	003 -	Remove any "*" that might be in the symbol name.
$ !		25-Jul-1989, DBS; Version V1-004
$ !	004 -	Added redirection of sys$error to nl: to suppress errors
$ !		when the symbol is not defined.
$ !		10-Nov-1989, DBS; Version V1-005
$ !	005 -	Made list file name unique.
$ !		30-Jul-1990, DBS; Version V1-006
$ !	006 -	Added call to disk check procedure.
$ !		12-Jun-1992, KPS; Version V1-007
$ !	007 -	Changed delete to deletee.
$ !		03-Aug-1998, DBS; Version V1-008
$ !	008 -	Changed defines to /nolog.
$ !-==========================================================================

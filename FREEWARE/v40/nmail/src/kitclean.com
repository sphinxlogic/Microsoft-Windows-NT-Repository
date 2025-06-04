$!
$! KITCLEAN.COM - called from KITINSTAL.COM to clean up
$! old files belonging to previous versions of Nmail.
$! (Only files which don't get replaced are deleted).
$!
$	call delfile vmi$root:[sysmgr]nm$start.com	! V9.0 startup proc
$	call delfile vmi$root:[sys$startup]nm$start.com	! V9.0 startup proc
$	exit vmi$_success
$
$!
$! Subroutine to check for file existence and delete where necessary
$! P1 = <full filespec>
$!
$delfile:         
$	subroutine
$	if f$parse(p1) .eqs. "" then exit 1
$	vmi$callback find_file nm$ 'p1' "" S nm$sts
$	if nm$sts .nes. "S" then exit 1
$	vmi$callback delete_file nm$
$	exit 1
$	endsubroutine
$
$!
$! End of KITCLEAN
$!

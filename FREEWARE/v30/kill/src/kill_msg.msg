	.TITLE	KILL_MSG
	.IDENT	/V1.0/
!++
!
!  File:	KILL_MSG.MSG
!  Author:	Darrell Burkhead
!		COPYRIGHT © 1993, MADGOAT SOFTWARE.  ALL RIGHTS RESERVED.
!  Date:	September 23, 1993
!
!  Description:
!
!	Message file for KILL.
!
!  Modification history:
!
!	V1.0	Darrell Burkhead	23-SEP-1993 14:55
!
!		Creation.
!--
	.FACILITY	KILL,1/PREFIX=KILL_

	.SEVERITY	INFORMATIONAL
KILLED		<Killed !AD>/FAO_COUNT=2
FORCEX		<Forced exit for !AD>/FAO_COUNT=2

	.SEVERITY	ERROR
INVUSER		<Invalid username !AS>/FAO_COUNT=1
INVPID		<Invalid PID !AS, specify a hexadecimal number>/FAO_COUNT=1
INVNODE		<Invalid node !AS>/FAO_COUNT=1
ERRINFO		<Error getting process information for process !XL>/FAO_COUNT=1
ERRKILL		<Error killing !AD>/FAO_COUNT=2
ERRFORCEX	<Error forcing exit for !AD>/FAO_COUNT=2

	.SEVERITY	WARNING
NOPROCS		<No matching processes were found>
SUICIDE		<Skipping process !XL>/FAO_COUNT=1

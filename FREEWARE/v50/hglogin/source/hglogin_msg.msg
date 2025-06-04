	.TITLE	HGLOGIN_MSG
	.FACILITY	HGLOGIN,102/prefix=HGLOGIN__
	.IDENT	/V1.4/
!
!  Copyright © 1997, 2000  Hunter Goatley.  All rights reserved.
!
!++
!
!  Facility:	HGLOGIN
!
!  Module:	HGLOGIN_MSG.MSG
!
!  Author:	Hunter Goatley
!
!  Date:	October 3, 1997
!
!  Modified by:
!
!	V1.4		Hunter Goatley		 9-AUG-2000 08:09
!		Add CTRLINV, INVCMDCHAR, HOWTOEXIT.
!
!	V1.0-3		Hunter Goatley		 8-JUN-2000 15:52
!		Add CREPRCERR.
!
!	V1.0-2		Hunter Goatley		16-OCT-1997 09:49
!		Add NOCMKRNL, NOSYSPRV.
!
!	V1.0		Hunter Goatley		 3-OCT-1997 22:35
!		Origin.
!--

.SEVERITY	FATAL
NOSUCHUSER	<no such user: !AS>/FAO=1
NOFTDEV		<no FTAx: pseudo-terminal found on the system>
NOCMKRNL	<HGLOGIN requires CMKRNL privilege>
NOSYSPRV	<HGLOGIN requires SYSPRV privilege>
PRCFINSTS	<process for user !AS terminated with an error>/FAO=1
CTRLINV		<reserved CTRL-key given as the ESCAPE_CHARACTER>
INVCMDCHAR	<invalid ESCAPE_CHARACTER value given; specify 0--31>

.SEVERITY	INFORMATIONAL
LOGIN		<logging in as user !AS, logout or press CTRL-!AS to exit>/fao=2
EXIT		<returning to original !AS session>/FAO=1

	.END

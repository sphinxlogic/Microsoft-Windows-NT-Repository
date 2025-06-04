	.TITLE	HGLOGIN_MSG
	.FACILITY	HGLOGIN,102/prefix=HGLOGIN__
	.IDENT	/V1.0-2/
!
!  Copyright © 1997, Hunter Goatley.  All rights reserved.
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

.SEVERITY	INFORMATIONAL
LOGIN		<logging in as user !AS>/fao=1
EXIT		<returning to original !AS session>/FAO=1

	.END

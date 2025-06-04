	.TITLE	GETCMD_MSG
	.IDENT	/01-002/
!++
!
!  File:	GETCMD_MSG.MSG
!
!  Author:	Hunter Goatley
!
!  Date:	June 2, 1992
!
!  Description:
!
!	Message file for GETCMD.B32.
!
!  Modification history:
!
!	01-002		Hunter Goatley		 2-JUN-1994 06:58
!		Added INVPID.
!
!	01-001		Hunter Goatley		 4-NOV-1992 11:38
!		Changed prefix from GETCMD_ to GETCMD__.
!
!	01-000		Hunter Goatley		 2-JUN-1992 09:35
!		Genesis.
!
!--
	.FACILITY	GETCMD,1/PREFIX=GETCMD__

	.SEVERITY	WARNING
NOCMDS		<no DCL commands to recall>

	.SEVERITY	ERROR
NOCLI		<no DCL CLI mapped for target process>
NODCL		<CLI mapped for target process is not DCL>
INVPID		<invalid PID specified>

	.END

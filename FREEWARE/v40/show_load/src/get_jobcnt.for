        SUBROUTINE GET_JOBCNT (P1,P2,P3,P4)
C******************************************************************************
C
C  Author	 : Curtis Rempel
C  Date		 : June 24, 1991
C  Last modified : June 24, 1991
C
C  File used	 : <none>
C
C  Purpose:
C
C	To retrieve the current interactive and batch job counts.
C
C******************************************************************************

	IMPLICIT	NONE		! catch any undeclared stuff

	INTEGER*2	P1		! interactive job count (SYS.MAP)
	INTEGER*2	P2		! batch job count (SYS.MAP)
	INTEGER*2	P3		! returned interactive job count
	INTEGER*2	P4		! returned batch job count

	P3 = P1
	P4 = P2

	RETURN

	END
